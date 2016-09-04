;; ---------------------------------------------------------------------
;; 
;; ---------------------------------------------------------------------

(define homedir (getenv "HOME"))
(define background_image_name (string-append homedir "/local/Texturen/MilkyWay/eso0932a2.png"))

(define r_obs 20.0 )
(define fov   60.0 )

;; --- Initialize spacetime metric
(init-metric '(type "SchwarzschildCart")
             '(mass 1.0)
             '(id "metric")
)

;(gvs-print '(id "metric"))

;; --- Initialize integrator for light rays
(init-solver '(type     "GSL_RK_Fehlberg")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(step_size 0.01)
             '(step_ctrl #t)
             '(id "raytracing")
)

;; --- Initialize 4pi camera
(init-camera '(type "4PICam")
             '(angle  90.0)
             '(res #( 100 50 ))
             ;'(res #( 360 180 ))
             ;'(res #( 1600 800 ))
             '(filter "FilterRGB")
             '(id "skycam")
)

;; --- Initialize ray generator
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX)  -70.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX     70.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector 0.0 r_obs 0.0 0.0 ))
              '(e0  #(1.0  0  0  0) )
              '(e1  #(0.0  1  0  0) )
              '(e2  #(0.0  0  1  0) )
              '(e3  #(0.0  0  0  1) )
              '(incoords #f) 
              '(id  "locTedObs")
)

(local-tetrad `(pos ,(vector 0.0 r_obs PIhalf 0.0 ))
              '(e0  #(1.0  0  0  0) )
              '(e1  #(0.0  0  0  1) )
              '(e2  #(0.0 -1  0  0) )
              '(e3  #(0.0  0 -1  0) )
              '(incoords #f) 
              '(id  "locTedSpObs")
)

;; --- Initialize projector with observer tetrad
(init-projector '(localTetrad "locTedObs")
                '(color #(0.0 0.0 0.0))
                ;'(errcolor #(1.0 0.0 0.0))
                ;'(cstrcolor #(1.0 0.0 0.0))
                ;'(breakcolor #(0.0 0.0 1.0))
                '(id "proj")
)

;; --- Set ambient light
(init-light-mgr '(ambient #(1.0 1.0 1.0)))


;; --- Set image texture for the background
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,background_image_name)
                                        `(transform ,(scale-obj #(-1.0 -1.0)))
                                        )
                         )
              '(ambient 1.0)
              '(id "bgShader")
)

;; --- Set sphere as representative for the background
(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(30.0 30.0 30.0))
                 `(transform ,(rotate-obj "z" 0.0))
                 '(shader "bgShader")
                 '(id "bgimage")
)

(comp-object '(obj "bgimage")
             '(id "scene")
)

(comp-octree-object '(obj "bgimage")
                    '(lower #(-1.0 -1.0 -1.0))
                    '(upper #(1.0 1.0 1.0))
                    '(subdivs 1)
                    '(id "oscene")
)

(init-device '(type "standard")
             '(obj "scene")
             '(camera "skycam")
             `(setparam ("metric" "mass" 0.5))
)

