;; ---------------------------------------------------------------------
;;  GeoViS:  nbinary.scm
;; ---------------------------------------------------------------------

(define incl  90.0 )


;; ---- Metrik ----
(init-metric '(type "Schwarzschild")
             '(mass 1.0)
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
             '(fov #(28.0 18.8125))
             '(res #(1024 688))
             '(filter "FilterRGBjac")
             '(id "cam")
)
;(gvs-print '(id "cam"))

;; ---- Strahlgenerator ----
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX)   0.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX    70.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; ---- Beobachter-Tetrade ----
(local-tetrad `(pos ,(vector 0.0 60.0 (* incl DEG_TO_RAD) 0.0 ))
              '(e0  #(1.0 0 0 0) )
              '(e1  #(0.0 -1  0  0) )
              '(e2  #(0.0  0  0  -1) )
              '(e3  #(0.0  0  -1 0) )
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

(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(2.5 2.5 2.5))
                 '(shader "checkShader")
                 '(id "nstar")
)


(init-texture '(type "UniTex")
              '(color #(0.8 0.16 0.16))
              '(id "utex1a")
)

(init-texture '(type "UniTex")
              '(color #(0.9 0.63 0.63))
              '(id "utex2a")
)

(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                                        '(texture "utex1a")
                                        '(texture "utex2a")
                                        `(transform ,(scale-obj #(20.0 10.0)))
                          )
               )
              '(ambient 0.2)
              '(diffuse 1.0)
              '(id "checkShader2")
)

(solid-ellipsoid `(objtype ,gpObjTypeLocal)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(3.0 3.0 3.0))
                 '(shader "checkShader2")
                 '(id "ostar")
)

(local-tetrad `(pos #(0.0 10.0 1.5707963 0.0))
              '(e0 #(1.0 0.0 0.0 0.0))
;              '(e1 #(0.0 1.0  0.0  0.0))
;              '(e2 #(0.0 0.0  0.0 -1.0))
;              '(e3 #(0.0 0.0 -1.0  0.0))
              '(e1 #(0.0 0.0 -1.0 0.0))
              '(e2 #(0.0 0.0 0.0 -1.0))
              '(e3 #(0.0 1.0 0.0 0.0))
              '(incoords #f) 
              '(id  "locTedOStar")
)

(local-comp-object '(obj "ostar")
                   '(localtetrad "locTedOStar")
                   ;'(motion "motion")
                   '(id "lco")
)


(comp-object '(obj "nstar")
             '(obj "lco")
             '(id "scene")
)


(do ((count 0 (+ count 1))) ((= count 5))
    (init-device '(type "standard")
                 '(obj "scene")
                 '(camera "cam")
                 `(setparam ("locTedOStar" "pos" ,(vector 0.0 10.0 1.5707963 (+ 0.0 (* 0.0174533 count)))))
    )
)

