;; ---------------------------------------------------------------------
;;  GeoViS:  morristhorne.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2013-11-21
;;
;;    A Morris-Thorne wormhole connects two universes. On both sides
;;    of the wormhole a 4pi image (equirectangular projection) are
;;    needed.
;; ---------------------------------------------------------------------

(define homedir (getenv "HOME"))
(define bg_image_up    (string-append homedir "/local/Texturen/Panorama/Tuebingen/marktplatz_tuebingen_2048.tif"))
(define bg_image_down  (string-append homedir "/local/Texturen/Panorama/Stuttgart/marktplatz_stuttgart_2048.tif"))

(init-metric '(type "MorrisThorne")
             '(b0 1.0)
             '(id "metric1")
)

(init-solver '(type "GSL_RK_Fehlberg")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(step_ctrl #t)
             '(step_size 0.01)
             '(id "raytracing")
)

(init-camera '(type "PinHoleCam")
             '(dir #( 1.0 0.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             '(fov #( 60.0 48.0 ))
             '(res #( 360 288 ))
             ;'(res #( 60 48 ))
             '(id "cam1")
)

(init-camera '(type "4PICam")
             '(res #( 200 100 ))
             '(id "cam2")
)

(init-raygen '(type    "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -130.0 -130.0 -130.0) )
             `(boundBoxUR  ,(vector  gpDBLMAX     130.0  130.0  130.0) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)


(local-tetrad '(pos #(0.0 3.0 1.5707963 0.0))
              '(e0  #(1.0  0  0  0) )
              '(e1  #(0.0 -1  0  0) )
              '(e2  #(0.0  0  0  1) )
              '(e3  #(0.0  0 -1  0) )
              '(incoords #f) 
              '(id  "locTedObs")
)
(init-projector '(localTetrad "locTedObs")
		    '(color #(0.0 0.0 0.0))
		    '(id "proj")
)

(init-light-mgr '(ambient #(1.0 1.0 1.0)))



(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,bg_image_up)
                                        `(transform ,(scale-obj #(-1.0 -1.0)))
                          )
               )
              '(ambient 1.0)
              '(id "bgShader")
)

(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(50.0 50.0 50.0))
                 '(shader "bgShader")
                 '(chart 0)
                 '(id "bgimage")
)

(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,bg_image_down)
                                        `(transform ,(scale-obj #(-1.0 1.0)))
                          )
               )
              '(ambient 1.0)
              '(id "bgShader2")
)

(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(50.0 50.0 50.0))
                 '(shader "bgShader2")
                 '(chart 1)
                 '(id "bgimage2")
)


(comp-object '(obj "bgimage")
             '(obj "bgimage2")
             '(id "scene")
)

(init-device '(type "standard")
             '(obj "scene")
             `(setparam ("locTedObs" "pos" ,(vector 0.0 1.0 1.5707963 0.0)))
             '(camera "cam2")
)


;(define my_step (/ 3.14159 180.0))
;(do ((count 0 (+ count 1))) ((= count 360))
;  (init-device '(type "standard")
;           '(obj "scene")
;	         `(setparam ("locTedObs" "pos" ,(vector 0.0 13.0 1.5707963 (* my_step count) )))
;	         '(camera "cam1")
;  )
;)

