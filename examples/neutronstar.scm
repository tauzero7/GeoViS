;; ---------------------------------------------------------------------
;;  GeoViS:  neutronstar.scm
;; ---------------------------------------------------------------------

(define homedir (getenv "HOME"))
(define bg_image (string-append homedir "/local/Texturen/MilkyWay/eso0932a2.png"))

(define r_obs 55.0 )
(define incl  90.0 )
(define fview 15.0 )

(define radius 3.0 )


;; ---- Metrik ----
(init-metric '(type "KerrBL")
             '(mass 1.0)
             '(angmom 1.0)
             '(id "metric")
)

;; ---- Geodaetenintegrator fuer Raytracing ----
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(step_size 0.01)
             '(id "raytracing")
)

;; ---- Beobachter-Kamera ----
(init-camera '(type "PinHoleCam")
             `(dir #( 1.0 0.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             `(fov ,(vector  fview fview ))
             ;'(res #(1024 1024))
             ;'(res #(50 50))
             '(res #(200 200))
             '(filter "FilterRGB")
             '(id "cam")
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
                '(color #(0.0 0.0 0.0))
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
                 `(axlen  ,(vector radius radius radius))
                 '(shader "checkShader")
                 '(id "star")
)

(comp-object '(obj "star")
             '(obj "bgimage")
             '(id "scene")
)


;(do ((count 0 (+ count 1))) ((= count 15))
;  (init-device '(type "standard")
;               '(obj "scene")
;               '(camera "cam")
;	       `(setparam ("metric" "mass" ,(+ 0.001 (* count 0.1))))
;  )
;)

(init-device '(type "standard")
             '(obj "scene")
)
         

