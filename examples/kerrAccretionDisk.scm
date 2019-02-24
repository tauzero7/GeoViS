;; ---------------------------------------------------------------------
;;  GeoViS:  kerrAccretionDisk.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2013-11-21
;;
;;    A thin disk around a Kerr black hole.
;;
;;    Kerr black hole:  M = 1,  a = 1.0
;;
;;    Observer
;;        radial position        r_obs = 50.0
;;        inclination            incl  = 80.0
;;        field of view          fview = 38.0
;;        view direction         vdir  =  0.0
;;
;;    Disk
;;        inner radius           r_in  = 3.0
;;        outer radius           r_out = 12.0
;;
;;
;;    1.) Image sequence with increasing inclination angle
;;    2.) Image suquence with increasing angular momentum a
;;
;;
;;    3.) Close-up views for m=1, a=1,
;;          .) incl = 80, fview = 5.0, vdir = 3.0
;;          .) incl = 80, fview = 1.5, vdir = 3.0
;;
;; ---------------------------------------------------------------------

(define r_obs 50.0 )
(define incl  80.0 )
(define fview 38.0 )
(define vdir  0.0 )

;; --- Initialize spacetime metric
(init-metric '(type "KerrBL")
             '(mass 1.0)
             '(angmom 1.0)
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
             `(dir ,(vector (cos (* vdir DEG_TO_RAD)) (sin (* vdir DEG_TO_RAD)) 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             `(fov ,(vector fview fview))
             '(res #(300 300))
             '(filter "FilterRGB")
             '(id "cam1")
)

(init-camera '(type "PinHoleCam")
             `(dir #( 1.0 0.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             `(fov ,(vector fview fview))
             '(res #(100 100))
             '(filter "FilterRGBpdz")
             '(id "cam2")
)

(init-camera '(type "2PICam") 
             '(heading -90.0)
             '(pitch    90.0)
             '(res  #(100 100))
             '(filter "FilterRGB")
             '(id "pancam")
)
             

;; --- Initialize ray generator
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX)   0.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX    70.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector 0.0 r_obs (* incl DEG_TO_RAD) 0.0 ))
              '(e0  #(1.0 0 0 0) )
              '(e1  #(0.0 -1  0  0) )
              '(e2  #(0.0  0  0  -1) )
              '(e3  #(0.0  0  -1 0) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; --- Initialize projector with observer tetrad
(init-projector '(localTetrad "locTedObs")
                '(color #(0.2 0.2 0.2))
                '(id "proj")
)

;; --- Set ambient light
(init-light-mgr '(ambient #(1.0 1.0 1.0)))

;; --- Set uniform texture 1 for disk shading
(init-texture '(type "UniTex")
              '(color #(0.8 0.16 0.16))
              '(id "utex1")
)

;; --- Set uniform texture 2 for disk shading
(init-texture '(type "UniTex")
              '(color #(0.9 0.63 0.63))
              '(id "utex2")
)

;; --- Set surface shader for sphere as checkerboard texture
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                                        '(texture "utex1")
                                        '(texture "utex2")
                                        `(transform ,(scale-obj #(30.0 12.0)))
                          )
               )
              '(ambient 0.4)
              '(diffuse 1.0)
              '(id "ringShader")
)

;; --- Set ring for accretion disk
(plane-ring `(objtype ,gpObjTypeInCoords)
            '(center #(0.0 0.0 0.0))
            '(normal #(0.0 0.0 1.0))
            '(rout  15.0)
            '(rin   3.0)
            '(shader "ringShader")
            '(id "ring")
)

;; --- The complete scene consists only of the accretion disk
(comp-object '(obj "ring")
             '(id "scene")
)

;; --- Generate image sequence
(define seq 1)

(if (= seq 1)
    (do ((count 1 (+ count 1))) ((= count 90))
        (init-device '(type "standard")
                     '(obj "scene")
                    '(camera "cam1")
                    `(setparam ("locTedObs" "pos" ,(vector 0.0 r_obs (* (- 90.0 count) DEG_TO_RAD) 0.0 )))
        )
    )
    (do ((count 1 (+ count 1))) ((= count 90))
        (init-device '(type "standard")
                     '(obj "scene")
                    '(camera "cam2")
                    `(setparam ("metric" "angmom" ,(* count 0.01)))
        )
    )
)

