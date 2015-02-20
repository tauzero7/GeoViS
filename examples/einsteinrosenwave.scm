;; ---------------------------------------------------------------------
;;  GeoViS:  einsteinrosenwave.scm
;; ---------------------------------------------------------------------

;; metric 
(init-metric '(type "EinsteinRosenWaveWWB")
             '(id "metric")
)

;; geodesic integrator for raytracing
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(step_ctrl #t)
             '(step_size 0.01)
             '(id "raytracing")
)
;(gvs-print '(id "raytracing"))


(init-camera '(type "PinHoleCam")
             '(dir #( 1.0 0.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             '(fov #( 60.0 45.0 ))
             ;'(res #(200 150))
             '(res #(100 75))
             '(filter "FilterRGB")
             '(id "cam")
)


(init-camera '(type "4PICam")
             '(angle  0.0)
             '(res #(800 400))
             '(filter "FilterRGB")
             '(id "pancam")
)

;; ray generator
(init-raygen '(type "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -100.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX    100.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
             '(id "rg")
)


;; observer tetrad
(local-tetrad '(pos #(0.0 30.0 0.0 0.0) )
              '(e0  #(1.0  0  0  0) )
              '(e1  #(0.0 -1  0  0) )
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

;(define background_image_name  "/home/tmueller/local/Bilder/Panorama/Tuebingen/marktplatz_tuebingen_2048.tif")
;(define background_image_name "examples/mwpan1600.tif")
(define background_image_name "/home/muelleta/local/Texturen/sphericalgrid.tif")
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
 
(do ((count 0 (+ count 1))) ((= count 120))           
    (init-device '(type "standard")
                 '(obj "scene")
                 '(camera "pancam")
                 `(setparam ("locTedObs" "time" ,(+ -30.0 (* count 0.5))))
    )
)

