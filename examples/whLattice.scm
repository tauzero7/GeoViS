; ---------------------------------------------------------------------
;   whLattice.scm
;
;   Morris-Thorne-Wurmloch verbindet zwei Raeume. Um den Wurmlochhals
;   ist auf beiden Seiten ein Gitter angebracht.
;
;  Thomas Mueller, Universität Stuttgart, VISUS
;
;   15.04.2008
;
;
;     Wurmloch-Schlund:   shapeconst = 2.0
;
; ---------------------------------------------------------------------

(init-metric '(type "MorrisThorne")
             '(b0 0.0001)
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
             '(fov #( 60.0 45.0 ))
             '(res #( 120 96 ))
             ;'(res #( 30 24 ))
             ;'(res #( 400 300 ))
             '(id "cam1")
)

(init-camera '(type "4PICam")
             '(res #( 2000 1000 ))
             '(id "cam2")
)

(init-raygen '(type    "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX) -130.0 -130.0 -130.0) )
             `(boundBoxUR  ,(vector  gpDBLMAX     130.0  130.0  130.0) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)


(local-tetrad '(pos #(0.0 8.0 1.5707963 1.5707963))
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


(init-light '(type "PointLight")
	    '(pos #(0.0 10.0 1.5707963 0.0))
	    '(color #(1.0 1.0 1.0))
	    '(atten #(0.0 0.0 0.0))
	    '(id "l1")
)

(init-light-mgr '(ambient #(1.0 1.0 1.0)))



;; -------------------------------------------------------------------------------
;;                              S z e n e r i e  1
;; -------------------------------------------------------------------------------
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

;(do ((countZ 0 (+ countZ 1))) ((= countZ 4))
;  (do ((countY 0 (+ countY 1))) ((= countY 4))
;    (do ((countX 0 (+ countX 1))) ((= countX 4))
;      (display (+ (* countZ 16) (+ (* countY 4) countX)))
;      (display "\n")
;    )
;  )
;)

;; ------------------------------------------------------
;;    Stange
;; ------------------------------------------------------
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                                        '(texture "texCyl0")
                                        '(texture "texCyl1")
                                        `(transform ,(scale-obj #(20.0 20.0)))
                                        )
                         )
              '(ambient 0.1)
              '(diffuse 1.0)
              '(specular 0.3)
              '(id "cylShader")
)


;; 48 Stangen entlang der z-Achse
  (do ((countY 0 (+ countY 1))) ((= countY 4))
    (do ((countX 0 (+ countX 1))) ((= countX 4))
      (solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector (- 12.0 (* countX 8.0)) (- 12.0 (* countY 8.0)) 12.0))
                  `(top      ,(vector (- 12.0 (* countX 8.0)) (- 12.0 (* countY 8.0))  4.0))
                  '(radii    #( 0.2 0.2 ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id ,(string-append "stangeZ1_" (number->string (+ (* countY 4) countX ))))
      )
      (solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector (- 12.0 (* countX 8.0)) (- 12.0 (* countY 8.0))  4.0))
                  `(top      ,(vector (- 12.0 (* countX 8.0)) (- 12.0 (* countY 8.0))  -4.0))
                  '(radii    #( 0.2 0.2 ))
                  '(shader   "cylShader")
                  '(chart 0)
                  `(id ,(string-append "stangeZ2_" (number->string (+ (* countY 4) countX ))))
      )
      (solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector (- 12.0 (* countX 8.0)) (- 12.0 (* countY 8.0))  -4.0))
                  `(top      ,(vector (- 12.0 (* countX 8.0)) (- 12.0 (* countY 8.0)) -12.0))
                  '(radii    #( 0.2 0.2 ))
                  '(chart 0)
                  '(shader   "cylShader")
                  `(id ,(string-append "stangeZ3_" (number->string (+ (* countY 4) countX ))))
      )
    )
  )
  
  

;; 48 Stangen entlang der y-Achse
  (do ((countZ 0 (+ countZ 1))) ((= countZ 4))
    (do ((countX 0 (+ countX 1))) ((= countX 4))      
      (solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector (- 12.0 (* countX 8.0)) 12.0 (- 12.0 (* countZ 8.0))))
                  `(top      ,(vector (- 12.0 (* countX 8.0))  4.0 (- 12.0 (* countZ 8.0))))
                  '(radii    #( 0.2 0.2 ))
                  '(chart 0)
                  '(shader   "cylShader")
                  `(id ,(string-append "stangeY1_" (number->string (+ (* countZ 4) countX ))))
      )
      (solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector (- 12.0 (* countX 8.0))  4.0 (- 12.0 (* countZ 8.0))))
                  `(top      ,(vector (- 12.0 (* countX 8.0)) -4.0 (- 12.0 (* countZ 8.0))))
                  '(radii    #( 0.2 0.2 ))
                  '(chart 0)
                  '(shader   "cylShader")
                  `(id ,(string-append "stangeY2_" (number->string (+ (* countZ 4) countX ))))
      )
      (solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector (- 12.0 (* countX 8.0)) -4.0 (- 12.0 (* countZ 8.0))))
                  `(top      ,(vector (- 12.0 (* countX 8.0)) -12.0 (- 12.0 (* countZ 8.0))))
                  '(radii    #( 0.2 0.2 ))
                  '(chart 0)
                  '(shader   "cylShader")
                  `(id ,(string-append "stangeY3_" (number->string (+ (* countZ 4) countX ))))
      )
    )
  )


;; 48 Stangen entlang der x-Achse
  (do ((countZ 0 (+ countZ 1))) ((= countZ 4))
    (do ((countY 0 (+ countY 1))) ((= countY 4))      
      (solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector 12.0 (- 12.0 (* countY 8.0)) (- 12.0 (* countZ 8.0))))
                  `(top      ,(vector  4.0 (- 12.0 (* countY 8.0)) (- 12.0 (* countZ 8.0))))
                  '(radii    #( 0.2 0.2 ))
                  '(chart 0)
                  '(shader   "cylShader")
                  `(id ,(string-append "stangeX1_" (number->string (+ (* countZ 4) countY ))))
      )
      (solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector  4.0 (- 12.0 (* countY 8.0)) (- 12.0 (* countZ 8.0))))
                  `(top      ,(vector -4.0 (- 12.0 (* countY 8.0)) (- 12.0 (* countZ 8.0))))
                  '(radii    #( 0.2 0.2 ))
                  '(chart 0)
                  '(shader   "cylShader")
                  `(id ,(string-append "stangeX2_" (number->string (+ (* countZ 4) countY ))))
      )
      (solid-cylinder   `(objType  ,gpObjTypeInCoords)
                  `(base     ,(vector -4.0 (- 12.0 (* countY 8.0)) (- 12.0 (* countZ 8.0))))
                  `(top      ,(vector -12.0 (- 12.0 (* countY 8.0)) (- 12.0 (* countZ 8.0))))
                  '(radii    #( 0.2 0.2 ))
                  '(chart 0)
                  '(shader   "cylShader")
                  `(id ,(string-append "stangeX3_" (number->string (+ (* countZ 4) countY ))))
      )
    )
  )

;; ------------------------------------------------------
;;    Kugel
;; ------------------------------------------------------
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "CheckerT2D")
                                        '(texture "texBall0")
                                        '(texture "texBall1")
                                        `(transform ,(scale-obj #(20.0 10.0)))
                                        )
                         )
              '(ambient 0.1)
              '(diffuse 1.0)
              '(specular 0.3)
              '(id "ballShader")
)


(do ((countZ 0 (+ countZ 1))) ((= countZ 4))
  (do ((countY 0 (+ countY 1))) ((= countY 4))
    (do ((countX 0 (+ countX 1))) ((= countX 4))
      (solid-ellipsoid `(objType ,gpObjTypeInCoords)
	      	 `(center ,(vector (- 12.0 (* countX 8.0)) (- 12.0 (* countY 8.0)) (- 12.0 (* countZ 8.0))))
		     '(axlen  #(0.5 0.5 0.5))
             '(chart 0)
             '(shader "ballShader")
		     `(id ,(string-append "ball_" (number->string (+ (* countZ 16) (+ (* countY 4) countX )))))
      )
    )
  )
)




(define background_image_name  "/export/home/tmueller/local/Texturen/MilkyWay/mwpan1600.tif")



; ----------------
;    Background 1
; ----------------
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,background_image_name)
                                        `(transform ,(scale-obj #(-1.0 -1.0)))
                                        )
                         )
              '(ambient 1.0)
              '(id "bgShader")
)

(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "UniTex")
                                        '(color #(0.0 0.0 0.0))
                                        )
                         )
              '(ambient 1.0)
              '(id "blackShader")
)



(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #(0.0 0.0 0.0))
                 '(axlen  #(50.0 50.0 50.0))
                 '(shader "blackShader")
                 '(chart 0)
                 '(id "bgimage")
)

; ----------------
;    Background 2
; ----------------
(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,background_image_name)
                                        `(transform ,(scale-obj #(-1.0 -1.0)))
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
            ; '(obj "bgimage2")
             '(id "scene")
)


;; Stangen hinzufuegen
(do ((countZ 0 (+ countZ 1))) ((= countZ 4))
  (do ((countY 0 (+ countY 1))) ((= countY 4))      
    (add-object '(add-to "scene")
           `(obj ,(string-append "stangeX1_" (number->string (+ (* countZ 4) countY ))))
           `(obj ,(string-append "stangeX2_" (number->string (+ (* countZ 4) countY ))))
           `(obj ,(string-append "stangeX3_" (number->string (+ (* countZ 4) countY ))))
           `(obj ,(string-append "stangeY1_" (number->string (+ (* countZ 4) countY ))))
           `(obj ,(string-append "stangeY2_" (number->string (+ (* countZ 4) countY ))))
           `(obj ,(string-append "stangeY3_" (number->string (+ (* countZ 4) countY ))))
           `(obj ,(string-append "stangeZ1_" (number->string (+ (* countZ 4) countY ))))
           `(obj ,(string-append "stangeZ2_" (number->string (+ (* countZ 4) countY ))))
           `(obj ,(string-append "stangeZ3_" (number->string (+ (* countZ 4) countY ))))
    )
  )
)

;; Kugeln hinzufuegen
(do ((countZ 0 (+ countZ 1))) ((= countZ 4))
  (do ((countY 0 (+ countY 1))) ((= countY 4))
    (do ((countX 0 (+ countX 1))) ((= countX 4))
      (add-object '(add-to "scene")
              `(obj ,(string-append "ball_" (number->string (+ (* countZ 16) (+ (* countY 4) countX )))))
      )
    )
  )
)

 

(init-device '(type "standard")
             '(obj "scene")
             `(setparam ("locTedObs" "pos" ,(vector 0.0 0.01 1.5707963 1.5707963)))
             '(camera "cam2")
)


;(define my_step (/ 3.14159 180.0))

;(do ((count 0 (+ count 1))) ((= count 360))
;  (init-device '(type "standard")
;           '(obj "scene")
;	         `(setparam ("locTedObs" "pos" ,(vector 0.0 13.0 1.5707963 (* my_step count) 0.0)))
;	         '(camera "cam1")
;  )
;)
