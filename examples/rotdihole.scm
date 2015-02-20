;; ---------------------------------------------------------------------
;;  GeoViS:  rotdihole.scm
;; ---------------------------------------------------------------------


(define x_obs -20.0 )
(define y_obs  0.0 )
(define z_obs  0.0 )
(define incl  90.0 )
(define fview 50.0 )

(define t_start 0.0 )
(define t_step  0.1 )
(define t_count 314 )


;; ---- Metrik ----
(init-metric '(type "RotDihole")
             '(mass1 0.1)
             '(mass2 0.1)
         '(omega 0.1)
         '(id "metric")
)

;; ---- Geodaetenintegrator fuer Raytracing ----
(init-solver '(type     "GSL_RK_Cash-Karp")
         '(geodType "lightlike")
         '(eps_abs  1.0e-9)
         '(id "raytracing")
)

;; ---- Beobachter-Kamera ----
(init-camera '(type "PinHoleCam")
         `(dir #( 1.0 0.0 0.0) )
         '(vup #( 0.0 0.0 1.0) )
         `(fov ,(vector  fview fview ))
;        '(res #(1000 1000))
         '(res #(10 10))
         '(filter "FilterRGB")
         '(id "cam1")
)

(init-camera '(type "PinHoleCam")
         `(dir #( 1.0 0.0 0.0) )
         '(vup #( 0.0 0.0 1.0) )
         `(fov ,(vector  fview fview ))
         '(res #(200 200))
         '(filter "FilterRGB")
         '(id "cam2")
)

;; ---- Strahlgenerator ----
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -70.0 (- gpDBLMAX) (- gpDBLMAX)) )
         `(boundBoxUR  ,(vector   gpDBLMAX    70.0  gpDBLMAX gpDBLMAX) )
         '(stepsize 0.01)
         '(solver "raytracing")
             '(maxNumPoints 5000)
)

;; ---- Beobachter-Tetrade ----
(local-tetrad `(pos ,(vector 0.0 x_obs y_obs z_obs ))
          '(e0  #(1.0  0  0  0) )
          '(e1  #(0.0  1  0  0) )
          '(e2  #(0.0  0  1  0) )
          '(e3  #(0.0  0  0  1) )
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


(define gvsObjTypeInCoords 2)
(define gvsObjTypeLocal 1)


; ----------------
;    Background
; ----------------
;(define background_image_name  "/home/muelleta/nobackup/Texturen/MilkyWay/mwpan1600.tif")
(define background_image_name  "/home/tmueller/local/Texturen/MilkyWay/mwpan16.tif")
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
                 `(transform ,(rotate-obj "z" 3.141))
                 '(shader "bgShader")
                 '(id "bgimage")
)

;; ------------------------------------------------------
;;          Gesamtszene
;; ------------------------------------------------------
(comp-object '(obj "bgimage")
             '(id "scene")
)

(init-device '(type "standard")
         '(obj "scene")
             '(camera "cam2")
         `(setparam ("locTedObs" "time" ,(+ t_start (* 0.5 10))))
)


;(do ((count 0 (+ count 1))) ((= count t_count))
;   (init-device '(type "standard")
;           '(obj "scene")
;           '(camera "cam2")
;           `(setparam ("locTedObs" "time" ,(+ t_start (* t_step count))))
;           )
;)
