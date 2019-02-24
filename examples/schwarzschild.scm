;; ---------------------------------------------------------------------
;;  GeoViS:  schwarzschild.scm
;;
;;  Author:  Thomas Mueller, University of Stuttgart, VISUS
;;           2013-12-06
;;
;;    Shadow of a black hole with Milky Way panorama.
;;
;;    Schwarzschild black hole:  M = 1,   rs = 2
;;
;;    Observer
;;        radial position        r_obs = 15rs
;;        
;;        pinhole camera ("cam") with field of view      'fov'
;;        panorama camera ("pancam") with field of view  360 x 120
;;        4pi camera ("skycam") with field of view       360 x 180
;;
;;    The bounding box for the scene is given in the spacetime's 
;;    intrinsic coordinates: (t,r,theta,phi). Here, only the radial
;;    range is limited to 0 < r < 70. 
;;
;;    You need a 4pi background texture, e.g. the Milky Way panorama
;;    by ESO/S.Brunier:
;;    http://www.eso.org/public/germany/images/eso0932a/    
;; ---------------------------------------------------------------------

(define homedir (getenv "HOME"))
(define background_image_name (string-append homedir "/local/Texturen/MilkyWay/eso0932a2.png"))
;(define background_image_name (string-append homedir "/local/Texturen/Sonstige/mosaic_135.png"))
;(define background_image_name "examples/eso0932a.tif")

(define r_obs 30.0 )
(define fov   60.0 )

;(m4d-metriclist)
;(m4d-solverlist)

;; --- Initialize spacetime metric
(init-metric '(type "Schwarzschild")
             '(mass 1.0)
             '(id "metric")
)


;; --- Initialize integrator for light rays
(init-solver '(type     "GSL_RK_Cash-Karp")
             '(geodType "lightlike")
             '(eps_abs  1.0e-8)
             '(step_size 0.01)
             '(step_ctrl #t)
             '(id "raytracing")
)

;; --- Initialize pinhole camera
(init-camera '(type "PinHoleCam")
             `(dir #( 0.0 1.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             `(fov ,(vector fov fov ))
             ;'(res #(8192 8192))
             ;'(res #(2048 2048))
             ;'(res #(350 350))
             '(res #(50 50))
             '(filter "FilterRGB")
             ;'(filter "FilterRGBIntersec")
             '(id "cam")
)

(init-camera '(type "PinHoleStereoCam")
             `(dir #( 0.0 1.0 0.0) )
             '(vup #( 0.0 0.0 1.0) )
             `(fov ,(vector fov fov ))
             '(res #(150 150))
             '(sep 1.0)
             '(filter "FilterRGB")
             '(id "stereocam")
)

;; --- Initialize panorama camera
(init-camera '(type "PanoramaCam")
             '(dir #( 0.0 1.0 0.0 ))
             '(vup #( 0.0 0.0 1.0 ))
             '(fov #( 360.0 120.0 ))
             '(res #( 360 120 ))
             '(filter "FilterRGB")
             '(id "panocam")
)

;; --- Initialize 4pi camera
(init-camera '(type "4PICam")
             '(angle  0.0)
             ;'(res #( 360 180 ))
             '(res #( 1600 800 ))
             '(filter "FilterRGB")
             '(id "skycam")
)

;; --- Initialize ray generator
(init-raygen '(type       "RayGenSimple")
             `(boundBoxLL  ,(vector (- gpDBLMAX)   0.0 (- gpDBLMAX) (- gpDBLMAX)) )
             `(boundBoxUR  ,(vector   gpDBLMAX    70.0  gpDBLMAX gpDBLMAX) )
             '(solver "raytracing")
             '(maxNumPoints 3000)
)

;; --- Set local reference frame of observer
(local-tetrad `(pos ,(vector 0.0 r_obs 1.5707963 0.0 ))
              '(e0  #(1.0  0  0  0) )
              '(e1  #(0.0  0  0  1) )
              '(e2  #(0.0 -1  0  0) )
              '(e3  #(0.0  0 -1  0) )
              '(incoords #f) 
              '(id  "locTedObs")
)

;; --- Initialize projector with observer tetrad
(init-projector '(localTetrad "locTedObs")
                '(color #(0.0 0.0 0.0))
                '(id "proj")
)

;; --- Set ambient light
(init-light-mgr '(ambient #(1.0 1.0 1.0)))


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
                 '(axlen  #(60.0 60.0 60.0))
                 `(transform ,(rotate-obj "z" 0.0))
                 '(shader "bgShader")
                 '(id "bgimage")
)

(comp-object '(obj "bgimage")
             '(id "scene")
)

(init-device '(type "standard")
             '(obj "scene")
             '(camera "cam")
             `(setparam ("metric" "mass" 0.5))
)

