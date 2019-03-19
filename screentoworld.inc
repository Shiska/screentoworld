/**
  * <summary>
  *   Projects a point from the screen to the global space and vice versa
  * </summary>
  *	<section>
  *   Credits
  *	</section>
  * <ul>
  *   Crayder
  *   <ul>
  *     <li>Widescreen detection</li>
  *   </ul>
  *   Ralfie
  *   <ul>
  *     <li>Bug in WorldToScreen</li>
  *   </ul>
  * </ul>
  *
  **//** */

// TODO: recheck https://forum.sa-mp.com/showthread.php?t=597368

#tryinclude <colandreas>

enum {
    Float: cInGameX = 0.7,
    Float: cInGameY = 0.525,
    Float: cInGameYw = 0.4, // Widescreen
    Float: cOnScreenX = 640.0,
    Float: cOnScreenY = 448.0
}
/// <summary>These values were gathered by assumption of the size of the screen</summary>

static stock NormCrossProduct(Float: v1x, Float: v1y, Float: v1z, Float: v2x, Float: v2y, Float: v2z, & Float: x, & Float: y, & Float: z) {
    ///	<summary>Calculates the CrossProduct and normalizes it</summary>
    /// <param name="v1x">First vector x</param>
    /// <param name="v1y">First vector y</param>
    /// <param name="v1z">First vector z</param>
    /// <param name="v2x">Second vector x</param>
    /// <param name="v2y">Second vector y</param>
    /// <param name="v2z">Second vector z</param>
    /// <param name="x">Output vector x</param>
    /// <param name="y">Output vector y</param>
    /// <param name="z">Output vector z</param>

    x = v1y * v2z - v2y * v1z;
    y = v1z * v2x - v2z * v1x;
    z = v1x * v2y - v2x * v1y;

    v1x = VectorSize(x, y, z);

    x /= v1x;
    y /= v1x;
    z /= v1x;
}

// native ScreenToWorld(playerid, Float: screenX, Float: screenY, & Float: vX, & Float: vY, & Float: vZ);
stock bool: ScreenToWorld(playerid, Float: screenX, Float: screenY, & Float: vX, & Float: vY, & Float: vZ) {
    ///	<summary>Converts point on screen (screenX, screenY) into a vector pointing into global space</summary>
    /// <export/>
    ///	<param name="playerid">The playerid</param>
    ///	<param name="screenX">X coordinate of the screen (0 - 640)</param>
    ///	<param name="screenY">Y coordinate of the screen (0 - 480)</param>
    ///	<param name="vX">Variable to store the X vector</param>
    ///	<param name="vY">Variable to store the Y vector</param>
    ///	<param name="vZ">Variable to store the Z vector</param>
    ///	<returns name="true">if the coordinates are on the screen and if the player is connected</returns>
    ///	<returns name="false">otherwise</returns>

	if((0.0 <= screenX <= cOnScreenX) && (0.0 <= screenY <= cOnScreenY) && GetPlayerCameraFrontVector(playerid, vX, vY, vZ)) {
		new Float: pX = ((screenX / cOnScreenX) - 0.5) * 2.0 * cInGameX;
		new Float: pZ = ((screenY / cOnScreenY) - 0.5) * 2.0 * ((GetPlayerCameraAspectRatio(playerid) > 1.375) ? cInGameYw : cInGameY);
		new Float: nXz;
		new Float: nYz;
		new Float: nZz;

		NormCrossProduct(vX, vY, vZ, vY, -vX, 0.0, nXz, nYz, nZz); // front vector (x) right vector

		vX += nXz * pZ + vY * pX;
		vY += nYz * pZ - vX * pX;
		vZ += nZz * pZ;
		
		return true;
	}
	return false;
}

#if defined COLANDREAS
    // native ScreenToWorldRayCast(playerid, Float: distance, Float: screenX, Float: screenY, & Float: wX, & Float: wY, & Float: wZ);
    stock ScreenToWorldRayCast(playerid, Float: distance, Float: screenX, Float: screenY, & Float: wX, & Float: wY, & Float: wZ) {
        ///	<summary>Converts point on screen (screenX, screenY) into the first intersection within the global space</summary>
        /// <export/>
        ///	<param name="playerid">The playerid</param>
        ///	<param name="screenX">X coordinate of the screen (0 - 640)</param>
        ///	<param name="screenY">Y coordinate of the screen (0 - 480)</param>
        ///	<param name="wX">Variable to store the intersection X</param>
        ///	<param name="wY">Variable to store the intersection Y</param>
        ///	<param name="wZ">Variable to store the intersection Z</param>
        /// <returns name="0">if the ray didn't collide with anything or if the coordinates aren't on the screen or if the player isn't connected</returns>
        /// <returns name="WATER_OBJECT">if the ray collided with water</returns>
        /// <returns name="modelid">The model of the object the ray collided</returns>
        /// <remarks>Only works if ColAndreas is included beforehands</remarks>

        if(ScreenToWorld(playerid, screenX, screenY, wX, wY, wZ)) {
            new Float: cX;
            new Float: cY;
            new Float: cZ;

            GetPlayerCameraPos(playerid, cX, cY, cZ);

            wX = cX + (wX * distance);
            wY = cY + (wY * distance);
            wZ = cZ + (wZ * distance);

            return CA_RayCastLine(cX, cY, cZ, wX, wY, wZ + 0.001, wX, wY, wZ) + 1;
        }
        return 0;
    }
#endif

// native WorldToScreen(playerid, Float: x, Float: y, Float: z, & Float: screenX, & Float: screenY);
stock bool: WorldToScreen(playerid, Float: x, Float: y, Float: z, & Float: screenX, & Float: screenY) {
    ///	<summary>Converts world coordinates to screen coordinates</summary>
    /// <export/>
    ///	<param name="playerid">The playerid</param>
    ///	<param name="x">X coordinate</param>
    ///	<param name="y">Y coordinate</param>
    ///	<param name="z">Y coordinate</param>
    ///	<param name="screenX">Variable to store the X coordinate of the screen</param>
    ///	<param name="screenY">Variable to store the Y coordinate of the screen</param>
    ///	<returns name="true">if the player is connected otherwise false</returns>
    ///	<returns name="false">otherwise</returns>

	new Float: cX;
	new Float: cY;
	new Float: cZ;

	if(GetPlayerCameraPos(playerid, cX, cY, cZ)) {
        new Float: vX;
        new Float: vY;
        new Float: vZ;
        new Float: nXz;
        new Float: nYz;
        new Float: nZz;

		GetPlayerCameraFrontVector(playerid, vX, vY, vZ);
		NormCrossProduct(vX, vY, vZ, vY, -vX, 0.0, nXz, nYz, nZz);
		// Distance, can be both positive and negative
		screenX = ( vX * (x - cX) + vY * (y - cY) + vZ * (z - cZ) ) / ((vX * vX) + (vY * vY) + (vZ * vZ));

		if((_: screenX & (1 << (cellbits - 1))) == 0) { // only positive values
			z = (((z - cZ) / screenX) - vZ) / nZz;
			x = (((x - cX) / screenX) - vX - (z * nXz)) / vY;

			screenX = ((x / (cInGameX * 2.0)) + 0.5) * cOnScreenX;
			screenY = ((z / (((GetPlayerCameraAspectRatio(playerid) > 1.375) ? cInGameYw : cInGameY) * 2.0)) + 0.5) * cOnScreenY;

			return true;
		}
	}
	return false;
}

// native WorldToScreenCheck(playerid, Float: x, Float: y, Float: z, & Float: screenX, & Float: screenY);
stock bool: WorldToScreenCheck(playerid, Float: x, Float: y, Float: z, & Float: screenX, & Float: screenY) {
    ///	<summary>Converts world coordinates to screen coordinates and checks if they are on the player screen</summary>
    /// <export/>
    ///	<param name="playerid">The playerid</param>
    ///	<param name="x">X coordinate</param>
    ///	<param name="y">Y coordinate</param>
    ///	<param name="z">Y coordinate</param>
    ///	<param name="screenX">Variable to store the X coordinate of the screen</param>
    ///	<param name="screenY">Variable to store the Y coordinate of the screen</param>
    ///	<returns name="true">if the coordinates are on the screen and if the player is connected</returns>
    ///	<returns name="false">otherwise</returns>

	return WorldToScreen(playerid, x, y, z, screenX, screenY) && (0.0 <= screenX <= cOnScreenX) && (0.0 <= screenY <= cOnScreenY);
}
