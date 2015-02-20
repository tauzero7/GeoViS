;; ---------------------------------------------------------------------
;;  GeoViS:  taubnut.scm
;; ---------------------------------------------------------------------

(define r_obs 50.0 )
(define incl  80.0 )

;; metric 
(init-metric '(type "TaubNUT")
             '(mass 1.0)
             '(l 0.1)
             '(id "metric")
)

;; geodesic integrator for raytracing
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(step_size 0.01)
             '(id "raytracing")
)

;; observer camera
(init-camera '(type "PinHoleCam")
             `(dir #( 1.0 0.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             '(fov #( 40.0 30.0 ))
             '(res #(640 480))
             '(filter "FilterRGBpdz")
             '(id "cam")
)

;; ray generator
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -100.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX    100.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; observer tetrad
(local-tetrad `(pos ,(vector 0.0 r_obs (* incl DEG_TO_RAD) 0.0 ))
              '(e0  #(1.0 0 0 0) )
              '(e1  #(0.0 -1  0  0) )
              '(e2  #(0.0  0  0  -1) )
              '(e3  #(0.0  0  -1 0) )
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

(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                                        '(texture "utex1")
                                        '(texture "utex2")
                                        `(transform ,(scale-obj #(20.0 4.0)))
                          )
               )
              '(ambient 0.4)
              '(diffuse 1.0)
              '(id "softCheckShader")
)

(define homedir (getenv "HOME"))
(define background_image_name (string-append homedir "/local/Texturen/MilkyWay/eso0932a.tif"))
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

(plane-ring `(objtype ,gpObjTypeInCoords)
            '(center #(0.0 0.0 0.0))
            '(normal #(0.0 0.0 1.0))
            '(rout  15.0)
            '(rin   6.0)
            '(shader "softCheckShader")
            '(id "ring")
)

(comp-object '(obj "bgimage")
             '(obj "ring")
             '(id "scene")
)

;(m4d-metriclist)


(do ((count 0 (+ count 1))) ((= count 100))
    (init-device '(type "standard")
                 '(obj "scene")
                 '(camera "cam")
                 `(setparam ("metric" "l" ,(* count 0.01)))
    )
)


