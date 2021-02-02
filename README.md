# 3D Basketball

github: https://github.com/fabianxvogt/3DBasketballGame

## Beschreibung
Simples 3D Basketball Spiel zum Körbe werfen. 

Die Turnhalle stammt aus folgendem Repos des Users bcedra:
https://github.com/bcedra/BasketballHallOpenGL

Ich habe die Kamerasteuerung angepasst, sodass man die Sicht eines Basketballspielers annimmt. Außerdem habe ich einen Ball eigebaut und ihn mit Textur, Fliegkräften und Gravitation versorgt. Das Blender-Projekt für den Ball habe ich kostenlos im Internet gefunden.

Ich habe alle Codestücke, die von mir stammen, in einen Kommentarblock nach folgendem Schema gekapselt:
/*
BEGIN: Basketball-Spiel: Beschreibung des Codeblocks
Author: Fabian Vogt (s0570800)
*/
<MY CODE>
/*
END: Basketball-Spiel: Beschreibung des Codeblocks
*/

## Steuerung
- W = Vorwärts gehen
- S = Rückwärts gehen
- A = Nach links gehen
- D = Nach rechts gehen
- SPACE = Springen
- R = Ball werfen
- Maus = Umschauen

## Ein paar Hinweise
- man kann sich frei innerhalb der Turnhalle bewegen, sich umschauen und rumspringen
- gerät man nah genug an den Ball heran, hebt man ihn auf
- hält man die R-Taste gedrückt, noch bevor man den Ball aufnimmt, so stößt/schießt man den Ball direkt von sich weg (siehe Video)

## TODOs
- Rotation des Balls mit Quaternionen fixen
- Punkte zählen und in einem GUI anzeigen


# BESCHREIBUNG DES ALTEN REPOS: BasketballHallOpenGL

## Description 
This application is implemented using OpenGL. Open Graphics Library is a programming language for graphical components. For creating the objects is used 3ds Max and for textures is used Photoshop CS6. The hall has many places for fans and 4 access doors and outside the hall, as a backdrop of the scenario, there is a winter theme skybox. The basketball hall has a parallelepiped shape with textures applied.

- camera movement using keyboard
- global lights
- moving lights
- shadows 
