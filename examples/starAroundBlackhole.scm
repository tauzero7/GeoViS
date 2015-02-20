;; ---------------------------------------------------------------------
;;  GeoViS:  starAroundBlackhole.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2013-11-21
;;
;;    A star orbits a Schwarzschild black hole on the last stable
;;    circular orbit. 
;;
;;    Schwarzschild black hole:  M = 1,   rs = 2
;;
;;    Observer
;;        radial position        r_obs = 15rs
;;        inclination            incl  = 90.0
;;        field of view          fview = 30.0
;;        observation times      t = t_start + count * t_step
;;                               for count = 0 ... t_count
;;
;;    Star
;;        orbital radius         r_obj = 3rs
;;        local velocity         lvel  = 0.5    
;;
;;    The bounding box for the scene is given in the spacetime's 
;;    intrinsic coordinates: (t,r,theta,phi). Here, only the radial
;;    range is limited to 0 < r < 50. 
;;
;;    Close-up view:   fview = 2.5
;;    
;; ---------------------------------------------------------------------

(define r_obs  30.0 )
(define incl   90.0 )
;(define fview  30.0 )
(define fview 2.5)

(define r_obj  6.0 )
(define lvel   0.5 )

(define t_start 27.89182 )
(define t_step  0.307812 )
(define t_count 301 )

;; --- Initialize spacetime metric
(init-metric '(type "Schwarzschild")
             '(mass 1.0)
             '(id "metric")
)

;; --- Initialize integrator for light rays
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(step_size 0.01)
             '(id "raytracing")
)

;; --- Initialize observer camera
(init-camera '(type "PinHoleCam")
             `(dir #( 1.0 0.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             `(fov ,(vector  fview fview ))
             '(res #(800 800))
             ;'(res #(1000 1000))
             '(filter "FilterRGB")
             '(id "cam1")
)

;; --- Initialize ray generator
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX)  0.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX   50.0    gpDBLMAX     gpDBLMAX ) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector  0.0 r_obs (* incl DEG_TO_RAD) 0.0 ))
              '(e0  #(1.0  0  0  0) )
              '(e1  #(0.0 -1  0  0) )
              '(e2  #(0.0  0  0 -1) )
              '(e3  #(0.0  0 -1  0) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; --- Initialize projector with observer tetrad
(init-projector '(localTetrad "locTedObs")
                '(color #(0.0 0.0 0.0))
                '(id "proj")
)

;; --- Set ambient light
(init-light-mgr '(ambient #(1.0 1.0 1.0)) )


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
              '(ambient 0.4)
              '(diffuse 1.0)
              '(id "ballShader")
)

(define homedir (getenv "HOME"))
(define background_image_name (string-append homedir "/local/Texturen/Erde/world.topo.bathy.200403.3x5400x2700.tif"))
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,background_image_name)
                                        `(transform ,(scale-obj #(1.0 -1.0)))
                                        )
                         )
              '(ambient 0.4)
              '(diffuse 1.0)
              '(id "ballShader2")
)

;; --- Set sphere as representative for star
(solid-ellipsoid `(objtype ,gpObjTypeLocal)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(0.5 0.5 0.5))
                 '(shader "ballShader2")
                 '(id "ball")
)

;; --- Initialize integrator for timelike geodesic
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "timelike")
             '(eps_abs  0.01)
             '(step_ctrl #f)
             '(step_size 0.1)
             '(id "gsolver")
)

;; --- Calculate timelike geodesic for star motion
(init-motion '(type "Geodesic")
             '(solver "gsolver")
             `(pos ,(vector 0.0 r_obj 1.5707963 0.0 ))
             `(localvel ,(vector 0.0 0.0 lvel))
             '(e0 #(1.0 0.0 0.0 0.0))
             '(e1 #(0.0 0.0 -1.0 0.0))
             '(e2 #(0.0 0.0 0.0 -1.0))
             '(e3 #(0.0 1.0 0.0 0.0))
             '(maxnumpoints 1000)
             '(forward  500.0)
             '(backward 500.0)
             '(id "motion")
)

(gvs-print-motion '(id "motion")
          '(file "motion.dat")
)

;; --- Set a static tetrad as alternative for star motion
(local-tetrad `(pos ,(vector 0.0 r_obj 1.5707963 0.0))
              '(e0 #(1.0 0.0 0.0 0.0))
              '(e1 #(0.0 0.0 -1.0 0.0))
              '(e2 #(0.0 0.0 0.0 -1.0))
              '(e3 #(0.0 1.0 0.0 0.0))
              '(incoords #f) 
              '(id  "locTedBall")
)

;; --- Combine star object and motion into one local object
(local-comp-object '(obj "ball")
                   '(localtetrad "locTedBall")
                   ;'(motion "motion")
                   '(id "lco1")
)

;; --- Generate image sequence
(do ((count 0 (+ count 1))) ((= count t_count))
    (init-device '(type "standard")
                 '(obj "lco1")
                 `(setparam ("locTedObs" "time" ,(+ t_start (* t_step count))))
    )
)

