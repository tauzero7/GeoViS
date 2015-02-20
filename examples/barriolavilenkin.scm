;; ---------------------------------------------------------------------
;;  GeoViS:  barriolavilenkin.scm
;; ---------------------------------------------------------------------

(define homedir (getenv "HOME"))
(define background_image_name (string-append homedir "/local/Texturen/MilkyWay/eso0932a.png"))

(define r_obs 60.0 )
(define incl  80.0 )

;; observer camera
(define viewAngle 0.0) 

;; metric 
(init-metric '(type "BarriolaVilenkin")
             '(k 1.0)
             '(id "metric")
)

;; geodesic integrator for raytracing
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-10)
             '(step_size 0.01)
             '(id "raytracing")
)

;(gvs-print '(id "raytracing"))


(init-camera '(type "PinHoleCam")
             `(dir ,(vector (cos (* viewAngle DEG_TO_RAD)) (sin (* viewAngle DEG_TO_RAD)) 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             '(fov #( 40.0 30.0 ))
             ;'(res #(200 150))
             '(res #(2048 1536))
             '(filter "FilterRGB")
             '(id "cam")
)

(init-camera '(type "4PICam")
             '(angle  0.0)
             '(res #(200 100))
             '(filter "FilterRGB")
             '(id "pancam")
)

(init-camera '(type "2PICam")
             '(heading 90.0)             
             '(pitch   50.0)
             '(res #(60 60))
             '(filter "FilterRGB")
             '(id "planetcam")
)


;; ray generator
(init-raygen '(type "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX)    0.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX    200.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; observer tetrad
(local-tetrad `(pos ,(vector 120.76 r_obs (* incl DEG_TO_RAD) 0.0 ))
              '(e0  #(1.0 0 0 0) )
              '(e1  #(0.0 -1  0  0) )
              '(e2  #(0.0  0  0  1) )
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
                                        `(transform ,(scale-obj #(5.0 24.0)))
                          )
               )
              '(ambient 0.2)
              '(diffuse 0.8)
              '(id "softCheckShader")
)

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

(define k_start 1.0)
(define k_step  0.01)

(do ((count 0 (+ count 1))) ((= count 100))                
    (init-device '(type "standard")
                 '(obj "scene")
                 '(camera "cam")
                 `(setparam ("metric" "k" ,(- k_start (* count k_step))))
    )
)

