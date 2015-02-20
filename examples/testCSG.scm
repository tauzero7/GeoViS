;; ---------------------------------------------------------------------
;;  GeoViS:  testCSG.scm
;; ---------------------------------------------------------------------

;; metric 
(init-metric '(type "Minkowski")
             '(id "metric")
)

;; geodesic integrator for raytracing
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-7)
             '(step_ctrl #f)
             '(step_size 10.0)
             '(id "raytracing")
)

;(gvs-print '(id "raytracing"))

;; observer camera
(define viewAngle 0.0) 

(init-camera '(type "PinHoleCam")
             ;`(dir ,(vector (cos (* viewAngle DEG_TO_RAD)) (sin (* viewAngle DEG_TO_RAD)) 0.0) )
             '(dir #(10.0 -1.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             '(fov #( 40.0 30.0 ))
             '(res #(200 150))
             ;'(res #(100 75))
             '(filter "FilterRGB")
             '(id "cam")
)

(init-camera '(type "4PICam")
             '(angle  0.0)
             '(res #(200 100))
             '(filter "FilterRGB")
             '(id "pancam")
)

;; ray generator
(init-raygen '(type "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -100.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX    100.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; observer tetrad
(local-tetrad `(pos ,(vector 0.0 -10.0 1.0 0.0 ))
              '(e0  #(1.0  0  0  0) )
              '(e1  #(0.0  1  0  0) )
              '(e2  #(0.0  0  1  0) )
              '(e3  #(0.0  0  0  1) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; projector
(init-projector '(localTetrad "locTedObs")
                '(color #(0.0 0.0 0.0))
                '(id "proj")
)

;; light sources manager
(init-light-mgr '(ambient #(1.0 1.0 1.0)))


;; ---------------------------------------------------------------
(init-texture '(type "UniTex")
              '(color #(0.8 0.16 0.16))
              '(id "utex1")
)

(init-texture '(type "UniTex")
              '(color #(0.9 0.63 0.63))
              '(id "utex2")
)

(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                         '(texture "utex1")
                         '(texture "utex2")
                         `(transform ,(scale-obj #(20.0 20.0)))
                         )
               )
              '(ambient 0.0)
              '(diffuse 1.0)
              '(id "ballShader")
)


(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                         '(texture "utex1")
                         '(texture "utex2")
                         `(transform ,(scale-obj #(40.0 20.0)))
                         )
               )
              '(ambient 1.0)
              '(diffuse 0.0)
              '(id "ballShader2")
)

(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(1.3 1.3 1.3))
                 `(transform ,(rotate-obj "z" 0.0))
                 '(shader "ballShader")
                 '(id "e1")
)

(solid-box `(objtype ,gpObjTypeInCoords)
           '(cornerll #(-1.0 -1.0 -1.0))
           '(cornerur #( 1.0  1.0  1.0))
           `(transform ,(rotate-obj "z" 20.0 (rotate-obj "x" 10.0 (rotate-obj "z" 30.0) )))
           '(shader "ballShader")
           '(id "box")
)

(solid-cylinder `(objtype ,gpObjTypeInCoords)
        '(base #(-1.5 0.0 0.0))
        '(top  #( 1.5 0.0 0.0))
        '(radii #( 0.5 0.5 ))
        '(shader "ballShader2")
        '(id "cy1")
)

(csg-object `(csgtype 1) 
            '(obj1 "e1")
            '(obj2 "cy1")
            '(id "csg")
)


;(define background_image_name  "/home/tmueller/local/Bilder/Panorama/Tuebingen/marktplatz_tuebingen_2048.tif")
(define background_image_name  "examples/marktplatz_tuebingen_2048.tif")
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,background_image_name)
                                        `(transform ,(scale-obj #(1.0 -1.0)))
                                        )
                         )
              '(ambient 1.0)
              '(id "bgShader")
)


(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(60.0 60.0 60.0))
                 `(transform ,(rotate-obj "z" 0.0))
                 '(shader "bgShader")
                 '(id "bgimage")
)

(comp-object '(obj "bgimage")
             '(obj "box")
             '(id "scene")
)
            
(init-device '(type "standard")
             '(obj "scene")
             '(camera "cam")
             ;'(camera "pancam")
             '(id "dev")
)
