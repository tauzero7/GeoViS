;; ---------------------------------------------------------------------
;;  GeoViS:  minkRotSphere.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2013-11-21
;;
;;     Special relativistic visualization of a rotating sphere.
;; ---------------------------------------------------------------------

;; --- Initialize spacetime metric
(init-metric '(type "Minkowski")
             '(id "metric")
)

;; --- Initialize integrator for light rays
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-7)
             '(step_ctrl #f)
             '(step_size 0.1)
             '(id "raytracing")
)

;; --- Initialize observer camera
(init-camera '(type "PinHoleCam")
             '(dir #(-1.0 0.0 0.0))
             '(vup #( 0.0 0.0 1.0))
             '(fov #( 10.0 10.0 ))
             '(res #(150 150))
             '(filter "FilterRGB")
             '(id "cam")
)

(init-camera '(type "PinHoleStereoCam")
             '(dir #(-1.0 0.0 0.0))
             '(vup #( 0.0 0.0 1.0))
             '(fov #( 10.0 10.0 ))
             '(res #(150 150))
             '(sep 0.5)
             '(filter "FilterRGB")
             '(id "stereocam")
)


;; --- Initialize ray generator
(init-raygen '(type "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -20.0 -20.0 -20.0 ))
             `(boundBoxUR  ,(vector   gpDBLMAX    20.0  20.0  20.0 ))
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector 0.0 15.0 0.0 0.0 ))
              '(e0  #(1.0  0.0  0.0  0.0) )
              '(e1  #(0.0  1.0  0.0  0.0) )
              '(e2  #(0.0  0.0  1.0  0.0) )
              '(e3  #(0.0  0.0  0.0  1.0) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; --- Initialize projector with observer tetrad
(init-projector '(localTetrad "locTedObs")
                '(color #(0.1 0.1 0.1))
                '(id "proj")
)

;; --- Set ambient light
(init-light-mgr '(ambient #(1.0 1.0 1.0))
                '(shadows #t)
)


;; --- Set uniform texture 1 for shading
(init-texture '(type "UniTex")
              '(color #(0.8 0.16 0.16))
              '(id "utex1")
)

;; --- Set uniform texture 2 for shading
(init-texture '(type "UniTex")
              '(color #(0.9 0.63 0.63))
              '(id "utex2")
)

;; --- Set surface shader as checkerboard texture
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                         '(texture "utex1")
                         '(texture "utex2")
                         `(transform ,(scale-obj #(20.0 10.0)))
                         )
               )
              '(ambient 0.2)
              '(diffuse 0.8)
              '(id "ballShader")
)

(init-motion '(type "ConstVelocity")
             '(boost #(0.0 0.0 0.0))
             '(rotation #(0.4 0.0 0.0 1.0))
             '(id "cm")
)

(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(1.0 1.0 1.0))
                 '(shader "ballShader")
                 `(transform ,(rotate-obj "y" 90.0))
                 '(motion "cm")
                 '(id "ball")
)

(comp-object '(obj "ball")
             '(id "scene")            
)


(define tmin  14.0)
(define tstep 0.1)
    
(do ((count 0 (+ count 1))) ((= count 157))
    (init-device '(type "standard")
                 '(obj "scene")
                 '(camera "cam")
                 `(setparam ("locTedObs" "time" ,(+ tmin (* tstep count)) ))
    )
)

