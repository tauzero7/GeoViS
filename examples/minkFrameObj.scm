;; ---------------------------------------------------------------------
;;  GeoViS:  minkFrameObj.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2015-01-03
;;
;;    
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
             '(step_size 0.05)
             '(id "raytracing")
)

;; --- Initialize observer camera
(init-camera '(type "PinHoleCam")
;             '(dir #(-15.0 4.0 -7.0))
             '(dir #(-1.0 0.0 0.0))
             '(vup #( 0.0 0.0 1.0))
             '(fov #( 10.0 10.0 ))
             '(res #( 30 30))
;             '(res #( 100 100))
             '(filter "FilterRGB")
             '(id "cam")
)


;; --- Initialize ray generator
(init-raygen '(type "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -70.0 -70.0 -70.0 ))
             `(boundBoxUR  ,(vector   gpDBLMAX    70.0  70.0  70.0 ))
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; --- Set local reference frame of observer
;(local-tetrad `(pos ,(vector 0.0 15.0 -4.0 7.0 ))
(local-tetrad `(pos ,(vector 0.0 45.0 0.0 0.0 ))
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
(init-light-mgr '(ambient #(1.0 1.0 1.0)))


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
              '(diffuse 0.9)
              '(id "ballShader")
)

(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(1.0 1.0 1.0))
                 ;'(axlen  #(0.5 0.5 0.5))
                 '(shader "ballShader")
                 `(transform ,(rotate-obj "y" 90.0))
                 '(id "ball")
)

(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(50.0 50.0 50.0))                
                 '(shader "ballShader")
                 ;`(transform ,(rotate-obj "y" 90.0))
                 '(id "background")
)

(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                         '(texture "utex1")
                         '(texture "utex2")
                         `(transform ,(scale-obj #(12.0 12.0)))
                         )
               )
              '(ambient 0.2)
              '(diffuse 0.8)
              '(id "frameShader")
)

(mesh-obj `(objtype ,gpObjTypeInCoords)
;          '(filename "box.obj")
;          '(pathname "/home/tmueller/local/Data/ObjModels/BoxFrame")
;          '(filename "die.obj")
;          '(pathname "/home/tmueller/local/Data/ObjModels/Die")
         '(filename "examples/sl_sphere.obj")
          '(pathname ".")
;          '(filename "letterT.obj")
;          '(pathname "/home/tmueller/Projekte/OGL4Core/Plugins/DefaultPlugins/BoxRenderer/resources/objects")
;	  `(transform ,(scale-obj #(1.0 1.0 1.0)))
          '(shader "frameShader")
          '(id "frame")
)

(comp-object '(obj "ball")
             '(obj "background")
             '(id "scene")            
)


(init-device '(type "standard")
;             '(obj "scene")
             '(obj "frame")
             '(camera "cam")
             `(setparam ("frame" "objfilename" "examples/test.obj"))
)

