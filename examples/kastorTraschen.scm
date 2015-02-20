;; ---------------------------------------------------------------------
;;  GeoViS:  kastorTraschen.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2013-11-21
;; ---------------------------------------------------------------------

(define fview 60.0)

;; --- Initialize spacetime metric
(init-metric '(type "KastorTraschen")
             '(h   0.02)
             '(m1  1.0)
             '(m2  1.0)
             '(z1  4.0)
             '(z2  -4.0)
             '(id "metric")
)
;(gvs-print '(id "metric"))

;; --- Initialize integrator for light rays
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-12)
             '(step_size 0.01)
             '(id "raytracing")
)

;; --- Initialize observer camera
(init-camera '(type "PinHoleCam")
             `(dir #( -1.0 0.0 0.0) )
             '(vup #(  0.0 0.0 1.0) )
             `(fov ,(vector fview fview))
             '(res #(400 400))
             '(filter "FilterRGB")
             '(id "cam")
)

;; --- Initialize ray generator
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -200.0 -200.0 -200.0) )
             `(boundBoxUR  ,(vector   gpDBLMAX    200.0  200.0  200.0) )
             '(solver "raytracing")
             '(maxNumPoints 6000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector 0.0 40.0 0.0 0.0 ))
              '(e0  #(1.0 0.0 0.0 0.0) )
              '(e1  #(0.0 1.0 0.0 0.0) )
              '(e2  #(0.0 0.0 1.0 0.0) )
              '(e3  #(0.0 0.0 0.0 1.0) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; --- Initialize projector with observer tetrad
(init-projector '(localTetrad "locTedObs")
                '(color #(0.0 0.0 0.0))
                '(id "proj")
)

;; --- Set ambient light
(init-light-mgr '(ambient #(1.0 1.0 1.0)))

;; --- Set surface texture for background sphere
(define homedir (getenv "HOME"))
;(define background_image_name (string-append homedir "/local/Texturen/MilkyWay/eso0932a.tif"))
(define background_image_name (string-append homedir "/local/Texturen/sphereImageMaster_white.tif"))
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
                 '(axlen  #(150.0 150.0 150.0))
                 `(transform ,(rotate-obj "z" 0.0))
                 '(shader "bgShader")
                 '(id "bgimage")
)

(comp-object '(obj "bgimage")
             '(id "scene")
)

;(calc-ray '(filename "ktray.txt")
;          '(pos  #(-30.0 30.0 0.0 0.0))
;          '(dir  #(176.0 90.0))
;)

(do ((count 0 (+ count 1))) ((= count 30))
    (init-device '(type "standard")
                 '(obj "scene")
                 '(camera "cam")
                 `(setparam ("locTedObs" "time" ,(+ (- 20.0) (* count 10.0)) ))
    )
)

