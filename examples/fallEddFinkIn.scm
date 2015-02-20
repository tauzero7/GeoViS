;; ---------------------------------------------------------------------
;;  Falling into a Schwarzschild black hole
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2013-11-21
;;
;;    An observer falls freely into a Schwarzschild black hole
;;    described in ingoing Eddington-Finkelstein coordinates.
;;
;;    At the initial position 'obsInitPos', the observer is at rest.
;;
;;    The apparent size of the black hole can be calculated by means
;;    of the formula (14) of [Gen.Relativ.Gravit 40,2185-2199 (2008)]:
;;
;;                      xobs^2 * sqrt(1-x0)*sqrt(xobs-x0) \pm sqrt(p(xobs^3-xobs^2+p))
;;     cos(ksi_crit) = ----------------------------------------------------------------
;;                                 xobs^3 - x0*xobs^2 + p
;;
;;    where p = 4/26, x0 = rs/r0, xobs = rs/robs.
;;
;;    rs = Schwarzschild radius
;;    r0 = initial position of observer: obsInitPos
;;    robs = current position of observer
;;
;;  The Milky Way panorama can be downloaded from
;;  http://www.eso.org/public/images/eso0932a/
;;  Note that you might have to convert the image into .ppm format.
;; ---------------------------------------------------------------------

;(define homedir (getenv "HOME"))
;(define background_image_name (string-append homedir "/local/Texturen/MilkyWay/eso0932a.tif"))
(define background_image_name "examples/eso0932a.tif")

(define  obsInitPos  10.0)
(define  objInitPos  6.0)

(init-metric '(type "EddFinkIn")
             '(mass 1.0)
             '(id "metric")
)

(init-solver '(type  "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs 1e-12)
             '(step_size 0.01)
             '(id "raytracing")
)

(init-camera '(type "PinHoleCam")
             '(dir #( 1.0 0.0 0.0 ))
             '(vup #( 0.0 0.0 1.0 ))
             '(fov #( 60.0 60.0 ))
             ;'(res #( 50 50 ))
             '(res #( 200 200 ))
             '(filter "FilterRGB")
             '(id "cam")
)

(init-camera '(type "PanoramaCam")
             `(dir #( 1.0 0.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             '(fov #( 360.0 60.0 ))
             '(res #( 360 60 ))
             ;'(res #(2400 400))
             '(filter "FilterRGB")
             '(id "pancam")
)


(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX)   0.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX   100.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 8000)
)

;; --- Initialize integrator for timelike geodesic
(init-solver '(type     "GSL_RK_Fehlberg")
             '(geodType "timelike")
             '(step_ctrl #f)
             '(step_size 0.005)
             '(id "gsolver")
)

;; --- Calculate timelike geodesic for star motion
(init-motion '(type "Geodesic")
             '(solver "gsolver")
             `(pos ,(vector 0.0  obsInitPos PIhalf 0.0 ))
             `(localvel ,(vector 0.0 0.0 0.0))
             '(e0 #( 1.0  0.0  0.0  0.0 ))
             '(e1 #( 0.0 -1.0  0.0  0.0 ))
             '(e2 #( 0.0  0.0  0.0  1.0 ))
             '(e3 #( 0.0  0.0 -1.0  0.0 ))
             '(maxnumpoints 8000)
             '(forward  100.0)
             '(backward 0.0)
             '(id "obsMotion")
)

(gvs-print-motion '(id "obsMotion")
          '(file "obsMotion.dat")
)


(local-tetrad `(pos ,(vector 0.0  obsInitPos PIhalf 0.0))
              '(e0 #( 1.0  0.0  0.0  0.0 ))
              '(e1 #( 0.0 -1.0  0.0  0.0 ))
              '(e2 #( 0.0  0.0  0.0  1.0 ))
              '(e3 #( 0.0  0.0 -1.0  0.0 ))
              '(incoords #f)
              '(id "locTedObsNat")
)

; ------------------------------------------------
;    textures and shaders
; ------------------------------------------------
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
                                        `(transform ,(scale-obj #(20.0 10.0)))
                          )
               )
              '(ambient 0.4)
              '(diffuse 1.0)
              '(id "checkShader")
)

(init-shader  '(type "SurfShader")
              `(objcolor ,(init-texture '(type "Image")
                                        `(file ,background_image_name)
                                        `(transform ,(scale-obj #(-1.0 -1.0)))
                                        )
                         )
              '(ambient 1.0)
              '(id "bgShader")
)

; ------------------------------------------------
;    background sphere
; ------------------------------------------------
(solid-ellipsoid `(objtype ,gpObjTypeInCoords)
                 '(center #( 0.0 0.0 0.0 ))
                 '(axlen  #(60.0 60.0 60.0 ))
                 `(transform ,(rotate-obj "z" 0.0))
                 '(shader "bgShader")
                 '(id "bgSphere")
)


; ------------------------------------------------
;    scene graph
; ------------------------------------------------
;(init-projector '(localTetrad "locTedObsNat")
(init-projector '(motion "obsMotion")
                '(color #( 0.0 0.0 0.0 ))
                '(id "proj")
)

(init-light-mgr '(ambient #( 1.0 1.0 1.0 )))


(comp-object '(obj "bgSphere")
             '(id "scene")
)


(define t_count  639)
(do ((count 0 (+ count 1))) ((= count t_count))
    (init-device '(type "standard")
                 '(obj "scene")
                 '(camera "pancam")
                 `(setparam ("proj" "actualpos" ,(* 11 count)))
    )
)



