A feladat elkezdésénél két út jutott az eszembe kapásból, egy implicit megoldás, amelynél valamilyen Ray Tracing algoritmus jeleníteném meg a gridet-et, ezt shaderben implementálva.
Ennél a megoldásnál nem kellett volna tárolni a pontok helyzetét és buffereket létrehozni, viszont valószínüleg nehezebb lett volna pontosan kiszámolni a kamera helyzetéből a 10-10 szegmenst.
A másik megoldás, ami mellett végül döntöttem, hogy a rácsot alkotó vonalak végpontjait kiszámolom és egy bufferbe betöltve kirajzoltatom őket a képernyőre. Ennek a legnagyobb előnyét abban
láttam, hogy mivel ismert lesz minden szakaszon két pont, ezért könnyen felírható a rá eső egyenes és így metszéspontot vagy egyéb dolgokat könnyebb lesz számolni a jövőben.
Ahhoz, hogy szabadon lehessen mozgatni később a kamerát, mindig a pozíciójából számolom ki, hogy melyik részét kell kirenderelni a rácsnak. Ezt nem úgy oldottam meg, hogy folyamatosan változik
a kirenderelt rács a kamera mozgásával, hanem a pozíciója alapján újraszámolja, hogy melyik vonalak láthatóak. Ez nem okoz gondot ebben a kameraszögben és nézetben, viszont ha a rács határait
látnánk, akkor a kamera mozgásával a megjelenítése a négyzetrácsnak szakadozós lenne, ha egy négyzetnyit megy akkor rajzolná újra.

A GeometryShader kódjának megírásához az alábbi linken található kódot vettem segítségül:
https://forum.unity.com/threads/issues-creating-fixed-width-lines-using-geometry-shader-in-view-space.442178/
