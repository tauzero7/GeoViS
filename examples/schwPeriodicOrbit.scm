;; ---------------------------------------------------------------------
;;  GeoViS:  schwPeriodicOrbit.scm
;;
;;  Author:  Thomas Mueller
;;           2020-05-02
;;
;;    A checkered sphere orbits a Schwarzschild black hole 
;;    on a periodic orbit. 
;;    (see CPC 181 (2010), 413-419)
;;
;;    Schwarzschild black hole:  M = 1,   rs = 2
;;
;;    Observer
;;        radial position        r_obs = ??
;;        inclination            incl  = 80.0
;;        field of view          fview = ??
;;        observation times      t = t_start + count * t_step
;;                               for count = 0 ... t_count
;;
;;    Sphere
;;        initial radius         r_init = 5 rs
;;        initial velocity       v_init = 0.3939  (local)
;;        initial direction      ksi_init = 68.2485 deg
;;
;;    The bounding box for the scene is given in the spacetime's 
;;    intrinsic coordinates: (t,r,theta,phi). Here, only the radial
;;    range is limited to 0 < r < 50. 
;;
;;    
;; ---------------------------------------------------------------------

(define homedir (getenv "HOME"))
(define background_image_name "examples/eso0932a.png")

(define mass 1.0)
(define rs     (* 2.0 mass))
(define r_obs  (* 20.0 rs))
(define incl   80.0)
(define vFoV  80.0)
;(define res #(120 80))
(define res #(480 320))

(define r_init   10.0)
(define v_init    0.3939)
(define ksi_init (- 180.0 68.2485))

(define sphere_radius  2.0)

;; ???
(define t_start 30.8 )
(define t_end   (+ t_start 1164.0))
(define t_count 300 )
(define t_step  (/ (- t_end t_start) (- t_count 1)))

(define aspect (/ (vector-ref res 0) (vector-ref res 1)))
(define hFoV  (calcHFoV (vector-ref res 0) (vector-ref res 1) vFoV))


;; --- Initialize spacetime metric
(init-metric '(type "Schwarzschild")
             `(mass 1.0)
             '(id "metric")
)

;; --- Initialize integrator for light rays
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-12)
             '(step_size 0.01)
             '(id "raytracing")
)

;; --- Initialize observer camera
(init-camera '(type "PinHoleCam")
             `(dir #( 0.0 1.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             `(fov ,(vector  hFoV vFoV))
             `(res ,res)
             '(filter "FilterRGB")
             '(id "cam1")
)

;; --- Initialize ray generator
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX)  0.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX   70.0    gpDBLMAX     gpDBLMAX ) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector  0.0 r_obs (* incl DEG_TO_RAD) 0.0 ))
              '(e0  #(1.0  0.0  0.0  0.0) )
              '(e1  #(0.0  0.0  0.0  1.0) )
              '(e2  #(0.0 -1.0  0.0  0.0) )
              '(e3  #(0.0  0.0 -1.0  0.0) )
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
              '(ambient 0.2)
              '(diffuse 1.0)
              '(id "sphereShader")
)

;; --- Set image texture for the background
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,background_image_name)
                                        `(transform ,(scale-obj #(-1.0 -1.0)))
                                        )
                         )
              '(ambient 1.0)
              '(diffuse 0.0)
              '(id "bgShader")
)

;; --- Set sphere as representative for the background
(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(60.0 60.0 60.0))
                 `(transform ,(rotate-obj "z" 0.0))
                 '(shader "bgShader")
                 '(id "bgimage")
)

;; --- Set sphere as representative for object
(solid-ellipsoid `(objtype ,gpObjTypeLocal)
                 '(center #(0.0 0.0 0.0))
                 `(axlen  ,(vector sphere_radius sphere_radius sphere_radius))
                 '(shader "sphereShader")
                 '(id "sphere")
)

;; --- Initialize integrator for timelike geodesic
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "timelike")
             '(eps_abs  0.01)
             '(step_ctrl #f)
             '(step_size 0.1)
             '(id "gsolver")
)

;; --- Calculate timelike geodesic for sphere motion
(define vx (* v_init (cos (radians ksi_init))))
(define vy (* v_init (sin (radians ksi_init))))
(init-motion '(type "Geodesic")
             '(solver "gsolver")
             `(pos ,(vector 0.0 r_init (* 90.0 DEG_TO_RAD) 0.0 ))
             `(localvel ,(vector vx 0.0 vy))
             '(e0 #(1.0 0.0 0.0 0.0))
             '(e1 #(0.0 0.0 -1.0 0.0))
             '(e2 #(0.0 0.0 0.0 -1.0))
             '(e3 #(0.0 1.0 0.0 0.0))
             '(maxnumpoints 20000)
             '(forward  1800.0)
             '(backward 1800.0)
             '(id "motion")
)

(gvs-print-motion '(id "motion")
          '(file "motion.dat")
)

;; --- Set a static tetrad as alternative for sphere motion
(local-tetrad `(pos ,(vector 0.0 r_init (* 90.0 DEG_TO_RAD) 0.0))
              '(e0 #(1.0 0.0 0.0 0.0))
              '(e1 #(0.0 0.0 -1.0 0.0))
              '(e2 #(0.0 0.0 0.0 -1.0))
              '(e3 #(0.0 1.0 0.0 0.0))
              '(incoords #f) 
              '(id  "locTedSphere")
)

;; --- Combine sphere object and motion into one local object
(local-comp-object '(obj "sphere")
                   ;'(localtetrad "locTedSphere")
                   '(motion "motion")
                   '(id "locObj")
)

(comp-object '(obj "locObj")
             '(obj "bgimage")
             '(id "scene")
)

;; --- Generate image sequence
(do ((count 0 (+ count 1))) ((= count t_count))
    (init-device '(type "standard")
                 '(obj "scene")
                 `(setparam ("locTedObs" "time" ,(+ t_start (* t_step count))))
    )
)