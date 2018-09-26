/**
  *	<subsection>
  *		<ul>
  *			Screen To World Include
  *			<p /><p />
  *			Legal
  *			<ul>
  *				The contents of this file are subject to the Mozilla Public License
  *				Version 1.1 (the "License"); you may not use this file except in
  *				compliance with the License. You may obtain a copy of the License at
  *				http://www.mozilla.org/MPL/
  *			</ul><ul>
  *				Software distributed under the License is distributed on an "AS IS"
  *				basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
  *				License for the specific language governing rights and limitations
  *				under the License.
  *			</ul><ul>
  *				The Original Code is Screen To World Include
  *			</ul><ul>
  *				The Initial Developer of the Original Code is Mark aka "Nero_3D".
  *				Portions created by Initial Developer are Copyright (C) 2013
  *				the Initial Developer. All Rights Reserved.
  *			</ul><ul>
  *				Credits:
  *				 - Crayder: Widescreen detection
  *				 - Ralfie: Bug in WorldToScreen
  *			</ul>
  *		</ul>
  *	</subsection>
  *
  **/

#if 0
	native ScreenToWorld(playerid, Float: screenX, Float: screenY, & Float: vX, & Float: vY, & Float: vZ);
	native ScreenToWorldEx(playerid, Float:distance, Float:screenX, Float:screenY, &Float:vX, &Float:vY, &Float:vZ);
	native WorldToScreen(playerid, Float: x, Float: y, Float: z, & Float: screenX, & Float: screenY);
#endif

/// These values were gathered by assumption of the size of the screen

const
	Float: cInGameX = 0.7,
	Float: cInGameY = 0.525,
	Float: cInGameYw = 0.4, // Widescreen
	Float: cOnScreenX = 640.0,
	Float: cOnScreenY = 448.0
;

///	<summary>Gets and normalizes the CrossProduct of the vectors</summary>

static stock STW_NormCrossProduct(& Float: x, & Float: y, & Float: z, Float: v1x, Float: v1y, Float: v1z, Float: v2x, Float: v2y, Float: v2z) {
	x = v1y * v2z - v2y * v1z;
	y = v1z * v2x - v2z * v1x;
	z = v1x * v2y - v2x * v1y;

	v1x = VectorSize(x, y, z);

	x /= v1x;
	y /= v1x;
	z /= v1x;
}

/**
  *	<summary>
  *		Converts screen to world coordinates
  *	</summary>
  *
  *	<param name="playerid">
  *		The playerid
  *	</param>
  *	<param name="screenX">
  *		X coordinate of the screen (0 - 640)
  *	</param>
  *	<param name="screenY">
  *		Y coordinate of the screen (0 - 480)
  *	</param>
  *	<param name="vX">
  *		Variable to store the X vector
  *	</param>
  *	<param name="vY">
  *		Variable to store the Y vector
  *	</param>
  *	<param name="vZ">
  *		Variable to store the Z vector
  *	</param>
  *
  *	<returns>
  *		True if the coordinates are on the screen and if the player is connected (returns vector)
  *	</returns>
  *
  **/
stock bool: ScreenToWorld(playerid, Float: screenX, Float: screenY, & Float: vX, & Float: vY, & Float: vZ) {
	if((0.0 <= screenX <= cOnScreenX) && (0.0 <= screenY <= cOnScreenY) && GetPlayerCameraFrontVector(playerid, vX, vY, vZ)) {
		new
			Float: pX = ((screenX / cOnScreenX) - 0.5) * 2.0 * cInGameX,
		#if defined GetPlayerCameraAspectRatio
			Float: pZ = ((screenY / cOnScreenY) - 0.5) * 2.0 * ((GetPlayerCameraAspectRatio(playerid) > 1.375) ? cInGameYw : cInGameY),
		#else
			Float: pZ = ((screenY / cOnScreenY) - 0.5) * 2.0 * cInGameY,
		#endif
//			Float: nXx, // vY
//			Float: nYx, // -vX
//			Float: nZx, // 0.0
			Float: nXz,
			Float: nYz,
			Float: nZz
		;
//		STW_NormCrossProduct(nXx, nYx, nZx, vX, vY, vZ, 0.0, 0.0, 1.0); // front vector (x) z vector
//		STW_NormCrossProduct(nXz, nYz, nZz, vX, vY, vZ, nXx, nYx, nZx); // front vector (x) right vector

//		vX += nXx * pX + nXz * pZ;
//		vY += nYx * pX + nYz * pZ;
//		vZ += nZx * pX + nZz * pZ;

		STW_NormCrossProduct(nXz, nYz, nZz, vX, vY, vZ, vY, -vX, 0.0); // front vector (x) right vector

		vX += nXz * pZ + vY * pX;
		vY += nYz * pZ - vX * pX;
		vZ += nZz * pZ;
		
		return true;
	}
	return false;
}

/**
  *	<summary>
  *		Converts screen to world coordinates
  *	</summary>
  *
  *	<param name="playerid">
  *		The playerid
  *	</param>
  *	<param name="distance">
  *		Distance factor
  *	</param>
  *	<param name="screenX">
  *		X coordinate of the screen (0 - 640)
  *	</param>
  *	<param name="screenY">
  *		Y coordinate of the screen (0 - 480)
  *	</param>
  *	<param name="vX">
  *		Variable to store the X vector or coordinate
  *	</param>
  *	<param name="vY">
  *		Variable to store the Y vector or coordinate
  *	</param>
  *	<param name="vZ">
  *		Variable to store the Z vector or coordinate
  *	</param>
  *
  *	<returns>
  *		True if the player is connected and if the coordinates are on the screen (returns world coordinates)
  *	</returns>
  *
  **/
stock bool: ScreenToWorldEx(playerid, Float: distance, Float: screenX, Float: screenY, & Float: wX, & Float: wY, & Float: wZ) {
	if(ScreenToWorld(playerid, screenX, screenY, wX, wY, wZ)) {
		new
			Float: cX,
			Float: cY,
			Float: cZ
		;
		GetPlayerCameraPos(playerid, cX, cY, cZ);

		wX = cX + (wX * distance);
		wY = cY + (wY * distance);
		wZ = cZ + (wZ * distance);

	#if defined COLANDREAS
		return CA_RayCastLine(cX, cY, cZ, wX, wY, wZ + 0.001, wX, wY, wZ) + 1;
	#endif
		return true;
	}
	return false;
}

/**
  *	<summary>
  *		Converts world to screen coordinates
  *	</summary>
  *
  *	<param name="playerid">
  *		The playerid
  *	</param>
  *	<param name="x">
  *		X coordinate
  *	</param>
  *	<param name="y">
  *		Y coordinate
  *	</param>
  *	<param name="z">
  *		Y coordinate
  *	</param>
  *	<param name="screenX">
  *		Variable to store the X coordinate of the screen
  *	</param>
  *	<param name="screenY">
  *		Variable to store the Y coordinate of the screen
  *	</param>
  *
  *	<returns>
  *		True if the coordinates are on the screen and if the player is connected
  *	</returns>
  *
  **/
stock bool: WorldToScreen(playerid, Float: x, Float: y, Float: z, & Float: screenX, & Float: screenY) {
	new
		Float: vX,
		Float: vY,
		Float: vZ,
		Float: cX,
		Float: cY,
		Float: cZ,
//		Float: nXx, // vY
//		Float: nYx, // -vX
//		Float: nZx, // 0.0
		Float: nXz,
		Float: nYz,
		Float: nZz
	;
	if(GetPlayerCameraPos(playerid, cX, cY, cZ)) {
		GetPlayerCameraFrontVector(playerid, vX, vY, vZ);

		// STW_NormCrossProduct(nXx, nYx, nZx, vX, vY, vZ, 0.0, 0.0, 1.0);
		// STW_NormCrossProduct(nXz, nYz, nZz, vX, vY, vZ, nXx, nYx, nZx);
		STW_NormCrossProduct(nXz, nYz, nZz, vX, vY, vZ, vY, -vX, 0.0);
		// Distance, can be both positive and negative
		screenX = ( vX * (x - cX) + vY * (y - cY) + vZ * (z - cZ) ) / ((vX * vX) + (vY * vY) + (vZ * vZ));

		if((_: screenX & (1 << (cellbits - 1))) == 0) { // only positive values
			z = (((z - cZ) / screenX) - vZ) / nZz;
			// x = (((x - cX) / screenX) - vX - (z * nXz)) / nXx;
			x = (((x - cX) / screenX) - vX - (z * nXz)) / vY;

			screenX = ((x / (cInGameX * 2.0)) + 0.5) * cOnScreenX;
		#if defined GetPlayerCameraAspectRatio
			screenY = ((z / (((GetPlayerCameraAspectRatio(playerid) > 1.375) ? cInGameYw : cInGameY) * 2.0)) + 0.5) * cOnScreenY;
		#else
			screenY = ((z / (cInGameY * 2.0)) + 0.5) * cOnScreenY;
		#endif
			return ((0.0 <= screenX <= cOnScreenX) && (0.0 <= screenY <= cOnScreenY));
		}
	}
	return false;
}
