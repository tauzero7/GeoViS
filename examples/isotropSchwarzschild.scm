;; ---------------------------------------------------------------------
;; 
;; ---------------------------------------------------------------------

(define homedir (getenv "HOME"))
;(define background_image_name (string-append homedir "/local/Texturen/MilkyWay/eso0932a2.png"))
(define background_image_name (string-append homedir "/local/Texturen/MilkyWay/ESA_Gaia_DR2_AllSky_Brightness_Colour_Cartesian_4000x2000.png"))

(define r_obs 50.0)
(define r_bg  100.0)

(define res #(180 90))
;(define res #(360 180))
;(define res #(1024 512))
(define fovY  20.0 )

(define aspect (/ (vector-ref res 0) (vector-ref res 1)))
(define fovX  (calcHFoV (vector-ref res 0) (vector-ref res 1) fovY))

(define y_start -20.0)
(define y_end    10.0)
(define t_count  300)
(define y_step  (/ (- y_end y_start) (- t_count 1)))
;(display t_step)
;(display "\n")


;; --- Initialize spacetime metric
(init-metric '(type "SchwarzschildIsotropic")
             '(mass 0.2)
             '(id "metric")
)

(gvs-print '(id "metric"))

;; --- Initialize integrator for light rays
(init-solver '(type     "GSL_RK_Fehlberg")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(step_size 0.01)
             '(step_ctrl #t)
             '(id "raytracing")
)

;; --- Initialize 4pi camera
(init-camera '(type "PinHoleCam")
             '(dir #(0.0 1.0 0.0))
             '(vup #(0.0 0.0 1.0))
             `(fov ,(vector fovX fovY))
             `(res ,res)
             '(filter "FilterRGB")
             '(id "skycam")
)

;; --- Initialize ray generator
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX)  (- gpDBLMAX) (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX   gpDBLMAX gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector 0.0 r_obs 0.0 0.0 ))
              '(e0  #(1.0  0.0  0.0  0.0) )
              '(e1  #(0.0  0.0  1.0  0.0) )
              '(e2  #(0.0 -1  0.0  0.0) )
              '(e3  #(0.0  0.0  0.0  1.0) )
              '(incoords #f) 
              '(id  "locTedObs")
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
                 `(axlen  ,(vector r_bg r_bg r_bg))
                 `(transform ,(rotate-obj "z" -30.0 (rotate-obj "x" -30.0)))
                 '(shader "bgShader")
                 '(id "bgimage")
)

(init-texture '(type "UniTex")
              '(color #(0.8 0.16 0.16))
              '(id "utex1")
)

(init-texture '(type "UniTex")
              '(color #(0.9 0.63 0.63))
              '(id "utex2")
)

(init-shader '(type "SurfShader")
             `(objcolor ,(init-texture '(type "CheckerT2D")
                                       '(texture "utex1")
                                       '(texture "utex2")
                                       `(transform ,(scale-obj #(10.0 10.0)))
                        )
             )
             '(ambient 0.1)
             '(diffuse 1.0)
             '(id "triShader")
)

(plane-triangle `(objtype ,gpObjTypeInCoords)
                '(p1 #(-10.0  0.0 -5.0))
                '(p2 #(-10.0 10.0 -5.0))
                '(p3 #(-10.0  0.0  5.0))
                '(shader "triShader")
                `(transform ,(translate-obj #(0.0 0.0 0.0)))
                '(id "triangle")
)                

(comp-object '(obj "bgimage")
             '(id "scene")
)

(add-object '(add-to "scene")
            '(obj "triangle")
)

;(init-device '(type "standard")
;             '(obj "scene")
;             '(camera "skycam")
;             `(setparam ("triangle" "transform" ,(translate-obj (vector 0.0 14.0 0.0))))
;)

(do ((count 0 (+ count 1))) ((= count t_count))
    (define ypos (+ y_start (* y_step count)))
    (init-device '(type "standard")
                '(obj "scene")
                '(camera "skycam")
                `(setparam ("triangle" "transform" ,(translate-obj (vector 0.0 ypos 0.0))))
    )
)