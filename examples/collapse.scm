; ---------------------------------------------------------------------
;  File:    collapse.scm
;  Author:  Thomas Mueller
;
;   Fake simulation of a collapsing star.
; ---------------------------------------------------------------------

(define homedir (getenv "HOME"))
(define bg_image (string-append homedir "/local/Texturen/MilkyWay/eso0932a2.png"))
(define star_image "2-stereoreache.png")

(define r_obs 35.0 )
(define incl  90.0 )
(define fview 45.0 )

;(define imageRes #(100 100))
(define imageRes #(512 512))

(define numImages 2)

(define mass_min 0.001)
(define mass_max 1.01)

(define radius_min 2.0)
(define radius_max 5.0)

(define xStep (/ 1.0 (- numImages 1)))

(define smoothStep (lambda (x)  (* (* x x) (+ 3.0 (* x (- 2))))))
(define interpolate (lambda (x xmin xmax) (+ (* xmin (- 1.0 x)) (* xmax x))))


;; ---- Metrik ----
(init-metric '(type "Schwarzschild")
             '(mass 1.0)
             '(id "metric")
)

;; ---- Geodaetenintegrator fuer Raytracing ----
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-10)
             '(step_size 0.01)
             '(id "raytracing")
)

;; ---- Beobachter-Kamera ----
(init-camera '(type "PinHoleCam")
             `(dir #( 1.0 0.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             `(fov ,(vector  fview fview ))
             `(res ,imageRes)
             '(filter "FilterRGB")
             '(id "cam")
)

(init-camera '(type "2PICam")
             `(heading 180.0)
             `(pitch  ,(+ 70.0))
             `(res ,imageRes)
             '(filter "FilterRGB")
             '(id "domecam")
)


;; ---- Strahlgenerator ----
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX)   0.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX    70.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 5000)
)

;; ---- Beobachter-Tetrade ----
(local-tetrad `(pos ,(vector 0.0 r_obs (* incl DEG_TO_RAD) 0.0 ))
              '(e0  #(1.0 0 0 0) )
              '(e1  #(0.0 -1  0  0) )
              '(e2  #(0.0  0  0 -1) )
              '(e3  #(0.0  0 -1  0) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; ---- Projektor ----
(init-projector '(localTetrad "locTedObs")
                '(color #(0.1 0.0 0.0))
                '(id "proj")
)

;; ---- Lichtquellen-Manager ----
(init-light-mgr '(ambient #(1.0 1.0 1.0)))


; ----------------
;    Background
; ----------------
(init-texture '(type "UniTex")
              '(color #(0.16 0.16 0.8))
              '(id "utex1")
)

(init-texture '(type "UniTex")
              '(color #(0.63 0.63 0.9))
              '(id "utex2")
)

(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                                        '(texture "utex1")
                                        '(texture "utex2")
                                        `(transform ,(scale-obj #(16.0 8.0)))
                          )
               )
              '(ambient 0.2)
              '(diffuse 1.0)
              '(id "checkShader")
)

(init-shader '(type "SurfShader")
             `(objcolor ,(init-texture '(type "Image")
                                       `(file ,star_image)
                                       `(transform ,(scale-obj #(1.0 1.0)))
                         )
              )
             '(ambient 0.3)
             '(diffuse 1.0)
             '(id "starShader")
)

;(define background_image_name "examples/checker.tif")
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,bg_image)
                                        `(transform ,(scale-obj #(-1.0 -1.0)))
                                        )
                         )
              '(ambient 1.0)
              '(id "bgShader")
)


(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(60.0 60.0 60.0))
                 `(transform ,(rotate-obj "z" 0.0))
                 '(shader "bgShader")
                 '(id "bgimage")
)

(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 `(axlen  ,(vector 2.0 2.0 2.0))
                 ;'(shader "checkShader")
                 '(shader "starShader")
                 '(id "star")
)

(comp-object '(obj "star")
             '(obj "bgimage")
             '(id "scene")
)


(do ((count 0 (+ count 1))) ((= count numImages))
    (define t (* count xStep))
    (define x (smoothstep t))
    (define nmass (interpolate x mass_min mass_max))
    (define nrad  (interpolate x radius_max radius_min))            
    (init-device '(type "standard")
                 '(obj "scene")
                 '(camera "domecam")
                 `(setparam ("bgimage" "transform" ,(rotate-obj "Z" 35.0 (rotate-obj "X" 30.0))))
                 `(setparam ("star" "axlen" ,(vector nrad nrad nrad)))
                 `(setparam ("metric" "mass" ,nmass))
    )
)
