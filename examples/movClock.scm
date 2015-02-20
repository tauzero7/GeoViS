; ---------------------------------------------------------------------
;  movClock.scm
; ---------------------------------------------------------------------

(init-metric '(type "Minkowski")
             '(id "metric")
)

(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-7)
             '(step_ctrl #f)
             '(id "raytracing")
)


(init-camera '(type "PanoramaCam")
         `(dir #( -1.0 0.0 0.0) )
         '(vup #( 0.0 0.0 1.0) )
         '(fov #( 120.0 30.0 ))
         '(res #( 100 25 ))
         '(filter "FilterRGB")
         '(id "cam1")
)

(init-camera '(type "PinHoleCam")
         `(dir #( -1.0 0.0 0.0) )
         '(vup #( 0.0 0.0 1.0) )
         '(fov #( 25.0 25.0 ))
         '(res #( 200 200 ))
         '(filter "FilterRGB")
         '(id "cam2")
)

(init-raygen '(type "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -100.0 -100.0 -100.0) )
             `(boundBoxUR  ,(vector   gpDBLMAX    100.0  100.0  100.0) )
             '(stepsize 5.0)
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

(local-tetrad '(pos #(0.0 10.0 0.0 0.0))
              '(e0  #(1.0  0  0  0) )
              '(e1  #(0.0  1  0  0) )
              '(e2  #(0.0  0  1  0) )
              '(e3  #(0.0  0  0  1) )
              '(incoords #f) 
              '(id  "locTedObs")
)

(init-projector '(localTetrad "locTedObs")
                    '(id "proj")
)

(init-light-mgr '(ambient #(1.0 1.0 1.0)))


; http://www.my-wallsticker.de/out/pictures/generated/product/1/515_355_100/ziffernblatt2.png
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        '(file "examples/ziffernblatt2.tif")
                                        `(transform ,(scale-obj #(1.0 1.0)))
                                        )
                         )
              '(ambient 0.9)
              '(artificial 1.0)
              '(id "cylShader")
)

(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "UniTex")
                                        '(color #(1.0 0.0 0.0))
                          )
               )
              '(ambient 0.9)
              '(artificial 1.0)
              '(id "zeigerShader")
)

(solid-cylinder `(objtype ,gpObjTypeLocal)
                '(base #(-0.1 0.0 0.0))
                '(top  #(-0.2 0.0 0.0))
                '(radii #(2.0 2.0))
                '(shader "cylShader")
                `(transform ,(rotate-obj "x" 1.5707963))
                '(id "uhr")
)


(init-motion '(type "ConstVelocity")
             '(rotation #(-0.9 1.0 0.0 0.0))
             '(id "constRotation")
)
 
(solid-cylinder `(objtype ,gpObjTypeLocal)
       '(base #(0.0 0.0 0.0))
       '(top  #(0.0 0.0 1.8))
       '(radii #(0.1 0.1))
       '(shader "zeigerShader")
       '(motion "constRotation")
       '(id "zeiger")
)

(local-tetrad '(pos #(0.0 0.0 0.0 0.0))
              '(incoords #f) 
              '(id  "locTedUhr")
)

(local-comp-object '(obj "uhr")
                   '(localtetrad "locTedUhr")
                   '(id "loUhr")
)

(comp-object '(obj "loUhr")
             '(id "scene")
)

(add-object '(add-to "scene")
            '(obj "zeiger")     
)



(init-device '(type "standard")
         '(obj "scene")
         '(camera "cam2")
         '(setparam ("locTedObs" "time" 4.5))
)


