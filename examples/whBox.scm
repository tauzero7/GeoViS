;; ---------------------------------------------------------------------
;;  GeoViS:  whBox.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2013-11-21
;;
;;    A box is located around a Morris-Thorne wormhole.
;;
;;    Wormhole throat size:   b0 = 2.0
;;
;;  The Milky Way panorama can be downloaded from
;;  http://www.eso.org/public/images/eso0932a/
;;  Note that you might have to convert the image into .ppm format.
;; ---------------------------------------------------------------------

;(define homedir (getenv "HOME"))
;(define background_image_name (string-append homedir "/local/Texturen/MilkyWay/eso0932a.tif"))
(define background_image_name "examples/eso0932a.tif")


;; --- Initialize spacetime metric
(init-metric '(type "MorrisThorne")
             '(b0 2.0)
             '(id "metric")
)

;; --- Initialize integrator for light rays
(init-solver '(type "GSL_RK_Fehlberg")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(step_ctrl #t)
             '(id "raytracing")
)

;; --- Initialize observer cameras
(init-camera '(type "PinHoleCam")
             '(dir #( 1.0 0.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             '(fov #( 50.0 40.0 ))
             '(res #( 120 96 ))
             ;'(res #(1000 800))
             ;'(res #(2000 1600))
             ;'(res #( 500 400 ))
             '(id "cam1")
)

(init-camera '(type "4PICam")
             '(res #( 200 100 ))
             '(id "cam2")
)

;; --- Initialize ray generator
(init-raygen '(type    "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -130.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector  gpDBLMAX     130.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; --- Set local reference frame of observer
(local-tetrad '(pos #(0.0 13.0 1.5707963 0.0))
              '(e0  #(1.0  0.0  0.0  0.0) )
              '(e1  #(0.0 -1.0  0.0  0.0) )
              '(e2  #(0.0  0.0  0.0  1.0) )
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


;; --- Set uniform textures for balls and rods
(init-texture '(type "UniTex")
              '(color #(0.376 0.459 0.325))
              '(id "texBall0")
)

(init-texture '(type "UniTex")
              '(color #(0.1376 0.1459 0.1325))
              '(id "texBall1")
)

(init-texture '(type "UniTex")
              '(color #(0.7 0.7 0.7))
              '(id "texCyl0")
)

(init-texture '(type "UniTex")
              '(color #(0.6 0.6 0.6))
              '(id "texCyl1")
)

;; --- Set rod shader
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                                        '(texture "texCyl0")
                                        '(texture "texCyl1")
                                        `(transform ,(scale-obj #(20.0 20.0)))
                                        )
                         )
              '(ambient 0.0)
              '(diffuse 1.0)
              '(id "cylShader")
)

(define cylRadius   0.1)
(define sphRadius   0.3)
(define baselength  2.0)

(solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector (- baselength) (- baselength) (- baselength)))
                  `(top      ,(vector (- baselength) (- baselength)    baselength))
                  `(radii    ,(vector cylRadius cylRadius ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id "s1")
)

(solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector (- baselength)     baselength (- baselength)))
                  `(top      ,(vector (- baselength)     baselength     baselength))
                  `(radii    ,(vector cylRadius cylRadius ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id "s2")
)

(solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector    baselength    baselength (- baselength)))
                  `(top      ,(vector    baselength    baselength     baselength))
                  `(radii    ,(vector cylRadius cylRadius ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id "s3")
)

(solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector    baselength (- baselength) (- baselength)))
                  `(top      ,(vector    baselength (- baselength)     baselength))
                  `(radii    ,(vector cylRadius cylRadius ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id "s4")
)

(solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector (- baselength) (- baselength) (- baselength)))
                  `(top      ,(vector     baselength (- baselength) (- baselength)))
                  `(radii    ,(vector cylRadius cylRadius ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id "s5")
)

(solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector     baselength (- baselength) (- baselength)))
                  `(top      ,(vector     baselength     baselength (- baselength)))
                  `(radii    ,(vector cylRadius cylRadius ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id "s6")
)

(solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector     baselength     baselength (- baselength)))
                  `(top      ,(vector (- baselength)     baselength (- baselength)))
                  `(radii    ,(vector cylRadius cylRadius ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id "s7")
)

(solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector (- baselength)     baselength (- baselength)))
                  `(top      ,(vector (- baselength) (- baselength) (- baselength)))
                  `(radii    ,(vector cylRadius cylRadius ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id "s8")
)

(solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector (- baselength) (- baselength)    baselength))
                  `(top      ,(vector     baselength (- baselength)    baselength))
                  `(radii    ,(vector cylRadius cylRadius ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id "s9")
)

(solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector     baselength (- baselength)    baselength))
                  `(top      ,(vector     baselength     baselength    baselength))
                  `(radii    ,(vector cylRadius cylRadius ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id "s10")
)

(solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector     baselength     baselength    baselength))
                  `(top      ,(vector (- baselength)     baselength    baselength))
                  `(radii    ,(vector cylRadius cylRadius ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id "s11")
)

(solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector (- baselength)     baselength    baselength))
                  `(top      ,(vector (- baselength) (- baselength)    baselength))
                  `(radii    ,(vector cylRadius cylRadius ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id "s12")
)

;; --- Set ball shader
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                                        '(texture "texBall0")
                                        '(texture "texBall1")
                                        `(transform ,(scale-obj #(20.0 10.0)))
                                        )
                         )
              '(ambient 0.4)
              '(diffuse 1.0)
              '(id "ballShader")
)

(solid-ellipsoid `(objType ,gpObjTypeInCoords)
             `(center ,(vector (- baselength) (- baselength) (- baselength)))
             `(axlen ,(vector sphRadius sphRadius sphRadius))
             '(chart 0)
             '(shader "ballShader")
             `(id "b1")
)

(solid-ellipsoid `(objType ,gpObjTypeInCoords)
             `(center ,(vector    baselength  (- baselength) (- baselength)))
             `(axlen ,(vector sphRadius sphRadius sphRadius))
             '(chart 0)
             '(shader "ballShader")
             `(id "b2")
)

(solid-ellipsoid `(objType ,gpObjTypeInCoords)
             `(center ,(vector    baselength     baselength  (- baselength)))
             `(axlen ,(vector sphRadius sphRadius sphRadius))
             '(chart 0)
             '(shader "ballShader")
             `(id "b3")
)

(solid-ellipsoid `(objType ,gpObjTypeInCoords)
             `(center ,(vector (- baselength)    baselength  (- baselength)))
             `(axlen ,(vector sphRadius sphRadius sphRadius))
             '(chart 0)
             '(shader "ballShader")
             `(id "b4")
)

(solid-ellipsoid `(objType ,gpObjTypeInCoords)
             `(center ,(vector (- baselength) (- baselength)    baselength))
             `(axlen ,(vector sphRadius sphRadius sphRadius))
             '(chart 0)
             '(shader "ballShader")
             `(id "b5")
)

(solid-ellipsoid `(objType ,gpObjTypeInCoords)
             `(center ,(vector    baselength  (- baselength)    baselength))
             `(axlen ,(vector sphRadius sphRadius sphRadius))
             '(chart 0)
             '(shader "ballShader")
             `(id "b6")
)

(solid-ellipsoid `(objType ,gpObjTypeInCoords)
             `(center ,(vector    baselength     baselength     baselength))
             `(axlen ,(vector sphRadius sphRadius sphRadius))
             '(chart 0)
             '(shader "ballShader")
             `(id "b7")
)

(solid-ellipsoid `(objType ,gpObjTypeInCoords)
             `(center ,(vector (- baselength)    baselength     baselength))
             `(axlen ,(vector sphRadius sphRadius sphRadius))
             '(chart 0)
             '(shader "ballShader")
             `(id "b8")
)

;; --- Set background shader
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,background_image_name)
                                        `(transform ,(scale-obj #(-1.0 -1.0)))
                                        )
                         )
              '(ambient 1.0)
              '(id "bgShader")
)

;; --- Set background sphere
(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(50.0 50.0 50.0))
                 '(shader "bgShader")
                 '(chart 0)
                 '(id "bgimage")
)

;; --- Scene graph consists of background image and objects
;; --- that are added below...
(comp-object '(obj "bgimage")
             '(id "scene")
)


;; --- add objects to scene graph
(add-object '(add-to "scene")
            '(obj "s1")
            '(obj "s2")
            '(obj "s3")
            '(obj "s4")
            '(obj "s5")
            '(obj "s6")
            '(obj "s7")
            '(obj "s8")
            '(obj "s9")
            '(obj "s10")
            '(obj "s11")
            '(obj "s12")
            '(obj "b1")
            '(obj "b2")
            '(obj "b3")
            '(obj "b4")
            '(obj "b5")
            '(obj "b6")
            '(obj "b7")
            '(obj "b8")
)

;; --- Generate image sequence
(do ((count 0 (+ count 1))) ((= count 360))
   (init-device '(type "standard")
                '(obj "scene")
                `(setparam ("locTedObs" "pos" ,(vector 0.0 13.0 1.5707963 (* count 0.01745) )))
                '(camera "cam1")
   )
)

