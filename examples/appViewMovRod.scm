;; ---------------------------------------------------------------------
;;  GeoViS:  appViewMovRod.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2014-06-10
;;
;;  A rod moving in the positive x-direction.
;;
;;  Observer is located at (x=0, y=0, z=0)
;;
;; ---------------------------------------------------------------------

(define tmin   -1.0)
(define tmax   0.14)
(define tstep  0.004)

(define beta   0.9)

(define gam  (/ 1.0 (sqrt (- 1.0 (* beta beta)))))

;; --- Initialize spacetime metric
(init-metric '(type "Minkowski")
             '(id "metric")
)

;; --- Initialize integrator for light rays
(init-solver '(type "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-7)
             '(step_ctrl #f)
             '(step_size 0.5)
             '(id "raytracing")
)

;; --- Initialize observer camera
(init-camera '(type "PinHoleCam")
             `(dir #( -1.0 0.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             '(fov #( 60.0 20.0 ))
             '(res #( 300 100 ))
             ;'(res #( 900 300 ))
             '(filter "FilterRGB")
             '(id "cam")
)

;; --- Initialize ray generator
(init-raygen '(type "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -30.0 -30.0 -30.0) )
             `(boundBoxUR  ,(vector   gpDBLMAX    30.0  30.0  30.0) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector 10.0 0.0 0.0 0.0))
              '(e0  #(1.0  0  0  0) )
              '(e1  #(0.0  1  0  0) )
              '(e2  #(0.0  0  1  0) )
              '(e3  #(0.0  0  0  1) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; --- Initialize projector with observer tetrad
(init-projector '(localTetrad "locTedObs")
                '(color #(0.1 0.1 0.1))
                '(id "proj")
)

;; --- Set ambient light
(init-light-mgr '(ambient #(1.0 1.0 1.0)))

;; --- Set uniform texture 1 for sphere shading
(init-texture '(type "UniTex")
              '(color #(0.8 0.16 0.16))
              '(id "utex1")
)

;; --- Set uniform texture 2 for sphere shading
(init-texture '(type "UniTex")
              '(color #(0.9 0.63 0.63))
              '(id "utex2")
)

;; --- Set surface shader for sphere as checkerboard texture
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                         '(texture "utex1")
                         '(texture "utex2")
                         `(transform ,(scale-obj #(20.0 10.0)))
                         )
               )
              '(ambient 0.2)
              '(diffuse 1.0)
              '(id "boxShader")
)

;; --- Set sphere as representative for ball
(solid-box `(objtype ,gpObjTypeLocal)
           '(cornerLL #(-0.1 -1.0 -0.1))
           '(cornerUR #(0.1 1.0 0.1))
           '(shader "boxShader")
           '(id "box")
)


;; --- Initialize integrator for timelike geodesic
(init-solver '(type "GSL_RK_Fehlberg")
             '(geodType "timelike")
             '(eps_abs  0.01)
             '(step_ctrl #f)
             '(id "gsolver")
)

;; --- Calculate timelike geodesic for motion
(init-motion '(type "Geodesic")
             '(solver "gsolver")
             `(pos ,(vector 0.0 0.0 0.0 0.0 ))
             `(localvel ,(vector beta 0.0 0.0))
             '(e0 #(1.0 0.0 0.0 0.0))
             '(e1 #(0.0 1.0 0.0 0.0))
             '(e2 #(0.0 0.0 1.0 0.0))
             '(e3 #(0.0 0.0 0.0 1.0))
             '(stepsize 0.1)
             '(maxnumpoints 3000)
             '(forward 200.0)
             '(backward 200.0)
             '(id "motion")
)

(local-tetrad '(pos #(0.0 0.0 0.0 0.0))
              '(localvel #(0.7071 0.0 0.0))
              '(incoords #f) 
              '(id  "locTed")
)

(local-comp-object '(obj "box")
                   ;'(localtetrad "locTed")
                   '(motion "motion")
                   '(id "locObj")
)

(comp-object '(obj "locObj")
             '(id "scene")
)


;; --- Generate image sequence
(define t_count (ceiling (/ (- tmax tmin) tstep)))
(display (string-append "t_count = " (number->string t_count) "\n"))

(do ((count 0 (+ count 1))) ((>= count t_count))    
    (define curr_t (+ tmin (* tstep count)))
    ;(display (string-append (number->string count) "  curr_t: " (number->string curr_t) "\n"))
    (init-device '(type "standard")
                 '(obj "scene")
                 '(camera "cam")
                 `(setparam ("locTedObs" "time" ,curr_t))
    )
)

