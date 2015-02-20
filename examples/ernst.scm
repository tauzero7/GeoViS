;; ---------------------------------------------------------------------
;;  GeoViS:  ernst.scm
;; ---------------------------------------------------------------------

;; ---- Metrik ----
(init-metric '(type "Ernst")
             '(b    0.01)
             '(mass 1.0)
             '(id "metric")
)

;(gvs-print '(id "metric"))

;; ---- Geodaetenintegrator fuer Raytracing ----
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(id "raytracing")
)

;; ---- Beobachter-Kamera ----
(init-camera '(type "PinHoleCam")
             `(dir #( 1.0 0.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             `(fov #( 60.0 60.0 ))
             '(res #(60 60))
             '(filter "FilterRGB")
             ;'(filter "FilterRGBjac")
             '(id "cam")
)

(init-camera '(type "PanoramaCam")
         `(dir #( -1.0 0.0 0.0) )
         '(vup #( 0.0 0.0 1.0) )
         '(fov #( 360.0 60.0 ))
         '(res #( 720 120 ))
         '(filter "FilterRGB")
         '(id "pancam")
)

;; ---- Strahlgenerator ----
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -100.0 -100.0 -100.0) )
             `(boundBoxUR  ,(vector   gpDBLMAX    100.0  100.0  100.0) )
             '(stepsize 0.01)
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; ---- Beobachter-Tetrade ----
(local-tetrad `(pos ,(vector 0.0 40.0 1.3707963 0.0 ))
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

(define homedir (getenv "HOME"))
(define background_image_name (string-append homedir "/local/Texturen/MilkyWay/mwpan16.ppm"))
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,background_image_name)
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

; ring
(init-texture '(type "UniTex")
              '(color #(0.8 0.16 0.16))
              '(id "utex1")
)

(init-texture '(type "UniTex")
              '(color #(0.9 0.63 0.63))
              '(id "utex2")
)

(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                                        '(texture "utex1")
                                        '(texture "utex2")
                                        `(transform ,(scale-obj #(20.0 10.0)))
                          )
               )
              '(ambient 1.0)
              '(id "checkShader")
)

(plane-ring `(objtype ,gpObjTypeInCoords)
            '(center #(0.0 0.0 0.0))
            '(normal #(0.0 0.0 1.0))
            '(rout  15.0)
            '(rin   6.0)
            '(shader "checkShader")
            '(id "ring")
)

(comp-object '(obj "bgimage")
             '(obj "ring")
             '(id "scene")
)

(do ((count 0 (+ count 1))) ((= count 15))
    (init-device '(type "standard")
                 '(obj "scene")
                 '(camera "cam")
                 `(setparam ("locTedObs" "time" ,(+ 20.0 (* count 1.0)) ))
    )
)

