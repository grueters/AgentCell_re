/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*

AgentCell is an agent-based simulation of bacterial chemotaxis.
Copyright (c) 2005 Trustees of the University of Chicago

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Authors: Thierry Emonet (emonet@uchicago.edu) and Michael J. North (north@anl.gov)

	Thierry Emonet
	920 E. 58th Street
	University of Chicago
	Chicago, IL 60637

	Michael J. North
	9700 S. Cass Avenue
	Argonne, IL 60439

*/
/*
 * Created on Feb 26, 2004
 *
 * 
 * 
 */
package agentCell_re.math;


//TODO: generate an error (e.g. RotationMatrixNotOrthogonalError) that can be used in the method isOrthogonal that we would have to overwrite from the class Matrix3x3 

/**
 * @author emonet
 *
 * This RotationMatrix are matrices with methods to setup rotations around the coordinate
 * axes and around a given direction vector
 */
public interface RotationMatrix extends Matrix {
    /**
    * @param axis: index of the axis of rotation
    * @param angle : angle of rotation
    * @return this : returns itself for convenience.
    * Assigns to THIS the rotation matrix corresponding to a
    * clockwise (CW) rotation of a point around one of the cartesian
    * angles (axis = 0, 1, or 2).
    * Example:
    *      If axis=0 then THIS=((1, 0, 0), (0, cos, -sin), (0, sin, cos))
    *      x' = THIS*x
    *      x = coordinates of point before CW rotation
    *      x' = coordinates of point after CW rotation
    *             (coordinate system unchanged)
    * Can also be interpreted as the coordinate transformation due
    * to a counterclockwise (CCW) rotation of the coordinate system:
    *         x' = coordinates of point in the new coordinate system
    *                rotated CCW with respect to the original one.
    */
    public RotationMatrix setAxisAngle(int axis, double angle);

    /**
     * @param v : axis of rotation vector (not necessarily of length 1)
     * @param angle : angle of rotation
     * @return this : returns itself for convenience.
     * Assigns to THIS the rotation matrix corresponding to a
     * clockwise (CW) rotation of a point around an axial vector v.
     * Example:
     *      If axis=0 then THIS=((1, 0, 0), (0, cos, -sin), (0, sin, cos))
     *      x' = THIS*x
     *      x = coordinates of point beforeCW rotation
     *      x' = coordinates of point after CW rotation
     *             (coordinate system unchanged)
     * Can also be interpreted as the coordinate transformation due
     * to a counterclockwise (CCW) rotation of the coordinate system:
     *         x' = coordinates of point in the new coordinate system
     *                rotated CCW with respect to the original one.
     */
    public RotationMatrix setAxisAngle(Vect v, double angle);

    // now the same but passing the cos and sin of the angles

    /**
     * @param axis: index of the axis of rotation
     * @param c : Cos(angle of rotation)
     * @param s : Sin(angle of rotation)
     * @return this : returns itself for convenience.
     * Assigns to THIS the rotation matrix corresponding to a
     * clockwise (CW) rotation of a point around one of the cartesian
     * angles (axis = 0, 1, or 2).
     * Example:
     *      If axis=0 then THIS=((1, 0, 0), (0, cos, -sin), (0, sin, cos))
     *      x' = THIS*x
     *      x = coordinates of point before CW rotation
     *      x' = coordinates of point after CW rotation
     *             (coordinate system unchanged)
     * Can also be interpreted as the coordinate transformation due
     * to a counterclockwise (CCW) rotation of the coordinate system:
     *         x' = coordinates of point in the new coordinate system
     *                rotated CCW with respect to the original one.
     */
    public RotationMatrix setAxisAngle(int axis, double c, double s);

    /**
     * @param v : axis of rotation vector (not necessarily of length 1)
     * @param c : Cos(angle of rotation)
     * @param s : Sin(angle of rotation)
     * @return this : returns itself for convenience.
     * Assigns to THIS the rotation matrix corresponding to a
     * clockwise (CW) rotation of a point around an axial vector v.
     * Example:
     *      If axis=0 then THIS=((1, 0, 0), (0, cos, -sin), (0, sin, cos))
     *      x' = THIS*x
     *      x = coordinates of point beforeCW rotation
     *      x' = coordinates of point after CW rotation
     *             (coordinate system unchanged)
     * Can also be interpreted as the coordinate transformation due
     * to a counterclockwise (CCW) rotation of the coordinate system:
     *         x' = coordinates of point in the new coordinate system
     *                rotated CCW with respect to the original one.
     */
    public RotationMatrix setAxisAngle(Vect v, double c, double s);

    /**
     * @param axesOrder : 0=>R0R1R2, 1=>R0R2R1, 2=>R1R0R2, 3=>R1R2R0, 4=>R2R0R1, 5=>R2R1R0
     * @param c0 : cos of angle for rotation around axis 0
     * @param c1 : cos of angle for rotation around axis 1
     * @param c2 : cos of angle for rotation around axis 2
     * @param s0 : sin  of angle for rotation around axis 0
     * @param s1 : sin  of angle for rotation around axis 1
     * @param s2 : sin  of angle for rotation around axis 2
     * @return this = R0R1R2, or R0R2R1, or ...
     * Composition of 3 axial rotations
     */
    public RotationMatrix setAxesAngles(int axesOrder, double c0, double c1,
        double c2, double s0, double s1, double s2);

    /**
     * @param axesOrder : 0=>R0R1R2, 1=>R0R2R1, 2=>R1R0R2, 3=>R1R2R0, 4=>R2R0R1, 5=>R2R1R0
     * @param angle0 : angle for rotation around axis 0
     * @param angle1 : angle for rotation around axis 1
     * @param angle2 : angle for rotation around axis 2
     * @return this = R0R1R2, or R0R2R1, or ...
     * Composition of 3 axial rotations
     */
    public RotationMatrix setAxesAngles(int axesOrder, double angle0,
        double angle1, double angle2);
}
