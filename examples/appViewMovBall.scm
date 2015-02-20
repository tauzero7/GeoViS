;; ---------------------------------------------------------------------
;;  GeoViS:  appViewMovBall.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2014-06-06
;;
;;  A ball moving in the positive x-direction.
;;
;;  Observer is located at (x=0, y=-10, z=0)
;;
;; ---------------------------------------------------------------------

(define tmin   0.0)
(define tmax  30.0)
(define tstep  0.1)

(define yobs  10.0)
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
             `(dir #( 0.0 1.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             '(fov #( 40.0 30.0 ))
             ;'(res #( 240 180 ))
             '(res #( 960 720 ))
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
(local-tetrad `(pos ,(vector 10.0 0.0 (- yobs) -1.0))
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
              '(id "ballShader")
)

;; --- Set sphere as representative for ball
(solid-ellipsoid `(objtype ,gpObjTypeLocal)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(1.0 1.0 1.0))
                 '(shader "ballShader")
                 `(transform ,(rotate-obj "x" 90.0))
                 '(id "ball")
)

(do ((countB 0 (+ countB 1))) ((= countB 13))
  (solid-ellipsoid `(objtype ,gpObjTypeLocal)
                   '(center #(0.0 0.0 0.0))
                   '(axlen #(1.0 1.0 1.0))
                   '(shader "ballShader")
                   `(transform ,(translate-obj (vector (+ -17.5 (* countB 2.5)) 0.0 -2.2) (rotate-obj "x" 90.0)))
                   `(id ,(string-append "ball_" (number->string countB)))
  )
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

(local-comp-object '(obj "ball")
                   ;'(localtetrad "locTed")
                   '(motion "motion")
                   '(id "locObj")
)

(comp-object '(obj "locObj")
             '(id "scene")
)

(do ((countB 0 (+ countB 1))) ((= countB 13))
  (add-object '(add-to "scene")
              `(obj ,(string-append "ball_" (number->string countB)))
  )
)


;(init-device '(type "standard")
;         '(obj "locObj")
;         '(camera "cam2")
;         '(setparam ("locTedObs" "time" 10.0))
;)

;; --- Generate image sequence
(define t_count (/ (- tmax tmin) tstep))

(define rad2deg (lambda (r) (* (/ r 3.14159265) 180.0)))


(do ((count 0 (+ count 1))) ((= count t_count))
    (define curr_t (+ tmin (* tstep count)))
    (define gbt (* gam (* beta curr_t)))
    (define w   (sqrt (+ (* gbt gbt) (* yobs yobs))))
    (define xs0 (- (* gam curr_t) w))
    (define app_x (* gam (* xs0 beta)))
    (define psi (atan (/ app_x yobs)))

    (display (string-append "psi: " (number->string (rad2deg psi)) "\n"))

    (init-device '(type "standard")
                 '(obj "scene")
                 '(camera "cam")
                 `(setparam ("locTedObs" "time" ,curr_t))
                 `(setparam ("cam" "dir" ,(vector (sin psi) (cos psi) 0.0)))
    )
)

