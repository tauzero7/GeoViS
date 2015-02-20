;; ---------------------------------------------------------------------
;;  GeoViS:  alcubierre.scm
;; ---------------------------------------------------------------------

(define r_obs -10.0 )

;; metric 
(init-metric '(type "AlcubierreWarpSimple")
             '(r 1.0)
             '(vs 1.0)
             '(id "metric")
)

;; geodesic integrator for raytracing
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-10)
             '(id "raytracing")
)

;; observer camera
(init-camera '(type "PinHoleCam")
             `(dir #( 0.0 1.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             '(fov #( 40.0 20.0 ))
             '(res #(400 200))
             '(filter "FilterRGB")
             '(id "cam")
)

;; ray generator
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -100.0 -100.0 -100.0) )
             `(boundBoxUR  ,(vector   gpDBLMAX    100.0  100.0  100.0) )
             '(stepsize 0.01)
             '(maxstep  0.1)
             '(solver "raytracing")
             '(maxNumPoints 5000)
)

;; observer tetrad
(local-tetrad `(pos ,(vector 10.0 0.0 r_obs 0.0 ))
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
;;    scene
;; ---------------------------------------------------------------

(define background_image_name  "/export/home/tmueller/local/Texturen/MilkyWay/mwpan1600.tif")
;(define background_image_name  "/home/muelleta/nobackup/Texturen/MilkyWay/mwpan1600.tif")
;(define background_image_name  "/home/tmueller/local/Texturen/MilkyWay/mwpan4.tif")
;(define background_image_name "examples/checker.tif")
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
                 `(transform ,(rotate-obj "z" -1.5707963))
                 '(shader "bgShader")
                 '(id "bgimage")
)


(comp-object '(obj "bgimage")
             '(id "scene")
)


(do ((count 0 (+ count 1))) ((= count 100))
    (init-device '(type "standard")
                 '(obj "scene")
                '(camera "cam")
                `(setparam ("metric" "vs" ,(* count 0.05)))
    )
)


