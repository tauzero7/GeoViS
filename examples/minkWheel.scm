;; ---------------------------------------------------------------------
;;  GeoViS:  minkWheel.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2013-11-21
;;
;;     Special relativistic visualization of a rolling wheel.
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
(define viewAngle -18.0) 
(init-camera '(type "PinHoleCam")
             `(dir ,(vector (cos (* viewAngle DEG_TO_RAD)) 0.0 (sin (* viewAngle DEG_TO_RAD)) ))
             `(vup ,(vector (- (sin (* viewAngle DEG_TO_RAD))) 0.0 (cos (* viewAngle DEG_TO_RAD)) ))
             '(fov #( 50.0 35.0 ))
             '(res #(1600 1120))
             ;'(res #(10 7))
             ;'(res #(100 70))
             '(filter "FilterRGB")
             '(id "cam")
)

;; --- Initialize ray generator
(init-raygen '(type "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -100.0 -100.0 -100.0 ))
             `(boundBoxUR  ,(vector   gpDBLMAX    100.0  100.0  100.0 ))
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector 0.0 -15.0 0.0 4.0 ))
              '(e0  #(1.0  0.0  0.0  0.0) )
              '(e1  #(0.0  1.0  0.0  0.0) )
              '(e2  #(0.0  0.0  1.0  0.0) )
              '(e3  #(0.0  0.0  0.0  1.0) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; --- Initialize projector with observer tetrad
(init-projector '(localTetrad "locTedObs")
                '(color #(0.0 0.0 0.0))
                '(id "proj")
)

(init-light '(type  "PointLight")
            '(pos   #(0.0 20.0 0.0 25.0))
            '(color #(1.0 1.0 1.0))
            '(atten #(0.0 0.0 0.0))
            '(id "pl1")
)

;; --- Set ambient light
(init-light-mgr '(ambient #(1.0 1.0 1.0))
                '(light "pl1")
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
                         `(transform ,(scale-obj #(10.0 10.0)))
                         )
               )
              '(ambient 0.2)
              '(diffuse 0.8)
              '(id "ballShader")
)

(init-motion '(type "ConstVelocity")
             ;'(boost #(0.0 -0.93 0.0))
             ;'(rotation #(0.465 1.0 0.0 0.0))
             '(boost #(0.0 -0.8 0.0))
             '(rotation #(0.4 1.0 0.0 0.0))
             '(id "cm")
)

(solid-cylinder `(objtype ,gpObjTypeInCoords)
                '(base #(-0.15 0.0 0.0))
                '(top  #( 0.15 0.0 0.0))
                '(radii #( 2.0 2.0 ))
                '(shader "ballShader")
                '(motion "cm")
                '(id "outerRing")
)

(solid-cylinder `(objtype ,gpObjTypeInCoords)
                '(base #(-0.152 0.0 0.0))
                '(top  #( 0.152 0.0 0.0))
                '(radii #( 1.8 1.8 ))
                '(shader "ballShader")
                '(motion "cm")
                '(id "innerRing")
)

(solid-cylinder `(objtype ,gpObjTypeInCoords)
                '(base #(-0.1 0.0 0.0))
                '(top  #( 0.1 0.0 0.0))
                '(radii #( 0.3 0.3 ))
                '(shader "ballShader")
                '(motion "cm")
                '(id "cy3")
)

(csg-object `(csgtype 1)
            '(obj1 "outerRing")
            '(obj2 "innerRing")
            '(id "csg")
)


(init-texture '(type "UniTex")
              '(color #(0.8 0.8 0.8))
              '(id "utex1a")
)

(init-texture '(type "UniTex")
              '(color #(0.3 0.3 0.3))
              '(id "utex2a")
)

(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                         '(texture "utex1a")
                         '(texture "utex2a")
                         `(transform ,(scale-obj #(10.0 10.0)))
                         )
               )
              '(ambient 0.2)
              '(diffuse 0.0)
              '(id "boxShader")
)

(solid-box `(objtype ,gpObjTypeInCoords)
           '(cornerll #(5.0 -9.0 -5.0))
           '(cornerur #(-5.0 9.0 -2.0))
           '(shader "boxShader")
           '(id "box")
)

(comp-object '(obj "csg")
             '(obj "cy3")            
             '(obj "box")
             '(id "scene")            
)


(do ((count 0 (+ count 1))) ((= count 8))    
    (define angle (* 45.0 count))
    (solid-cylinder `(objtype ,gpObjTypeInCoords)
                    '(base #(0.0 0.0 0.0))
                    '(top  #(0.0 0.0 1.9))
                    '(radii #(0.05 0.05))
                    '(shader "ballShader")
                    '(motion "cm")
                    ;`(transform ,(rotate-obj "x" (+ (* 45.0 count) 0.0) ))
                    `(transform ,(rotate-obj "x" angle ))
                    `(id ,(string-append "spoke_" (number->string count)))
    )
    (add-object '(add-to "scene")
                `(obj ,(string-append "spoke_" (number->string count)))
    )   
)

;(define tmin  5.0)
;(define tmax 17.0)
(define tmin  6.3)
(define tmax 28.7)

(define tstep  (/ (- tmax tmin) 224.0))
    
(do ((count 0 (+ count 1))) ((= count 224))      
    (init-device '(type "standard")
                 '(obj "scene")
                '(camera "cam")
                ;'(camera "pancam")
                `(setparam ("locTedObs" "time" ,(+ tmin (* tstep count)) ))
    )
)

