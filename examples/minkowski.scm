;; ---------------------------------------------------------------------
;;  GeoViS:  minkowski.scm
;; ---------------------------------------------------------------------

(define r_obs 4.0 )
(define incl  80.0 )

;; metric 
(init-metric '(type "Minkowski")
             '(id "metric")
)

;; geodesic integrator for raytracing
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-7)
             '(step_ctrl #f)
             '(step_size 10.0)
             '(id "raytracing")
)

;(gvs-print '(id "raytracing"))

;; observer camera
(define viewAngle -10.0) 

(init-camera '(type "PinHoleCam")
             `(dir ,(vector (cos (* viewAngle DEG_TO_RAD)) (sin (* viewAngle DEG_TO_RAD)) 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             '(fov #( 60.0 45.0 ))
             '(res #(200 150))
             ;'(res #(100 75))
             '(filter "FilterRGB")
             '(id "cam")
)

(init-camera '(type "4PICam")
             '(angle  0.0)
             '(res #(360 180))
             '(filter "FilterRGB")
             '(id "pancam")
)

(init-camera '(type "2PICam")
             '(heading   0.0)             
             '(pitch    60.0)
             '(res #(100 100))
             '(filter "FilterRGB")
             '(id "planetcam")
)

(init-camera '(type "PanoramaCam")
             '(dir #(1.0 0.0 0.0))
             '(vup #(0.0 0.0 1.0))
             '(fov #(360.0 120.0))
             '(res #(360 120))
             '(filter "FilterRGB")
             '(id "panocam")
)


;; ray generator
(init-raygen '(type "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -100.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX    100.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; observer tetrad
(local-tetrad `(pos ,(vector 120.76 r_obs (* incl DEG_TO_RAD) 0.0 ))
              '(e0  #(1.0 0 0 0) )
              '(e1  #(0.0  1  0  0) )
              '(e2  #(0.0  0  1  0) )
              '(e3  #(0.0  0  0  1) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; projector
(init-projector '(localTetrad "locTedObs")
                '(color #(0.0 0.0 0.0))
                '(id "proj")
)

;; light sources manager
(init-light-mgr '(ambient #(1.0 1.0 1.0)))


;; ---------------------------------------------------------------

(define background_image_name  "/home/tmueller/local/Bilder/Panorama/Tuebingen/marktplatz_tuebingen_2048.png")
;(define background_image_name  "examples/marktplatz_tuebingen_2048.tif")
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,background_image_name)
                                        `(transform ,(scale-obj #(1.0 -1.0)))
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

(comp-object '(obj "bgimage")
             '(id "scene")
)
            
(init-device '(type "standard")
             '(obj "scene")
             '(camera "panocam")
             ;'(camera "pancam")
             ;'(camera "planetcam")
             `(setparam ("planetcam" "heading" 0.0))
             '(id "dev")
)
