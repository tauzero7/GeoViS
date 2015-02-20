;; ---------------------------------------------------------------------
;;  GeoViS:  mtContactRings.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2014-07-31
;;
;;     
;; ---------------------------------------------------------------------

(define homedir (getenv "HOME"))
(define background_image_name (string-append homedir "/local/Texturen/MilkyWay/eso0932a2.png"))


;; --- Initialize spacetime metric
(init-metric '(type "MorrisThorne")
             '(b0 1.0)
;(init-metric '(type "Schwarzschild")
;             '(mass 0.5)
             '(id "metric")
)

;; --- Initialize integrator for light rays
(init-solver '(type     "GSL_RK_Fehlberg")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(step_ctrl #t)
             '(step_size 0.1)
             '(max_step 1.0)
             '(id "raytracing")
)

;; --- Initialize observer camera
(define viewAngle -18.0) 
(init-camera '(type "PinHoleCam")
             '(dir #( 0.0 1.0 0.0 ))
             '(vup #( 0.0 0.0 1.0 ))
             '(fov #( 25.0 25.0 ))
             '(res #(100 100))
             '(filter "FilterRGB")
             '(id "cam")
)

;; --- Initialize ray generator
(init-raygen '(type "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -100.0  (- gpDBLMAX) (- gpDBLMAX) ))
             `(boundBoxUR  ,(vector   gpDBLMAX    100.0  gpDBLMAX  gpDBLMAX ))
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector 0.0 12.0 PIhalf 0.0 ))
              '(e0  #(1.0  0.0  0.0  0.0) )
              '(e1  #(0.0  0.0  0.0  1.0) )
              '(e2  #(0.0 -1.0  0.0  0.0) )
              '(e3  #(0.0  0.0 -1.0  0.0) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; --- Initialize projector with observer tetrad
(init-projector '(localTetrad "locTedObs")
                '(color #(0.2 0.2 0.2))
                '(id "proj")
)


;; --- Set ambient light
(init-light-mgr '(ambient #(1.0 1.0 1.0))
                '(shadows #t)
)


;; --- Set uniform texture 1 for shading
(init-texture '(type "UniTex")
              ;'(color #(0.8 0.16 0.16))
              '(color #(0.9 0.63 0.63))
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
                         `(transform ,(scale-obj #(10.0 4.0)))
                         )
               )
              '(ambient 0.2)
              '(diffuse 0.8)
              '(id "ringShader")
)

(init-motion '(type "ConstVelocity")
             '(rotation #(0.2 0.0 3.0 0.0))
             '(id "cm")
)

(define radiusOut   2.0)
(define radiusIn    1.7)
(define half   0.05)
(define eps    0.01)


;(solid-cylinder `(objtype ,gpObjTypeInCoords)
                ;`(base ,(vector (- half) 0.0 0.0))
                ;`(top  ,(vector  half 0.0 0.0))
                ;`(radii ,(vector radiusOut radiusOut ))
                ;'(chart 0)
                ;'(shader "ringShader")
                ;'(motion "cm")
                ;'(id "outerRing")
;)

;(solid-cylinder `(objtype ,gpObjTypeInCoords)
                ;`(base ,(vector (- (- half) eps) 0.0 0.0))
                ;`(top  ,(vector  (+ half eps) 0.0 0.0))
                ;`(radii ,(vector radiusIn radiusIn ))
                ;'(chart 0)
                ;'(shader "ringShader")
                ;'(motion "cm")
                ;'(id "innerRing")
;)

;(csg-object `(csgtype ,gpCSGDifferObj)
            ;'(obj1 "outerRing")
            ;'(obj2 "innerRing")
            ;'(id "csg")
;)

(solid-ring `(objtype ,gpObjTypeInCoords)
            `(base ,(vector (- half) 0.0 0.0))
            `(top  ,(vector    half  0.0 0.0))
            `(radii ,(vector radiusOut radiusOut))
            `(innerradii ,(vector radiusIn radiusIn))
            '(chart 0)
            '(shader "ringShader")
            '(motion "cm")
            '(id "ring")
)

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
                 '(axlen  #(80.0 80.0 80.0))
                 '(chart 1)
                 `(transform ,(rotate-obj "z" 0.0))
                 '(shader "bgShader")
                 '(id "bgimage")
)


(comp-object '(obj "ring")
             '(obj "bgimage")
             '(id "scene")            
)
    

(init-device '(type "standard")
             '(obj "scene")
             '(camera "cam")
             ;'(camera "pancam")
             `(setparam ("locTedObs" "time" 32.8 ))
             `(setparam ("ring" "transform" ,(rotate-obj "X" 0.0 (rotate-obj "Y" 95.0))))
)


