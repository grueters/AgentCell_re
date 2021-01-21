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
 * Created on Feb 24, 2004
 *
 * 
 * 
 */
package agentCell_re.math;

import javax.vecmath.AxisAngle4f;
import javax.vecmath.Matrix3f;

import repast.simphony.random.RandomHelper;

/**
 * @author emonet
 *
 *         Implementation of the Orientation interface with matrices
 *
 *         The orientation O is a 3x3 rotation matrix that transforms the
 *         coordinates of a point from the body referential (attached to the
 *         body) into the laboratory referential (fixed). The column vectors of
 *         O are the coordinates (in the laboratory referential) of the basis
 *         vectors of the body referential. x = coordinates of a point in the
 *         body referential x' = coordinates of a point in the laboratory
 *         referential O = orientation x' = O . x
 *
 *         If we apply on the body a rotation R around a global (laboratory)
 *         axis, we obtain: x_new = x remains unchanged: the point is fixed in
 *         the body x'_new = R . x' = R .O . x = new coordinates in laboratory
 *         referential O_new = R . O = new orientation of the body
 *
 *         If we apply on the body a rotation R around one of its local axes we
 *         obtain: x_new = x, remains unchanged: the point is fixed in the body
 *         x'_new = O . R . x = new coordinates in laboratory referential O_new
 *         = O . R = new orientation of the body
 */
public class Orientation extends RotationMatrix3x3 {
	// rotation matrix used internally. It is never accessed by other objects.
	private RotationMatrix rotationMatrix = new RotationMatrix3x3();
	private int rotationsCount = 0;
	private int rotationsCountThreshold = 100;

	/**
	 *
	 */
	public Orientation() {
		super();
	}

	/**
	 * @param xx
	 * @param xy
	 * @param xz
	 * @param yx
	 * @param yy
	 * @param yz
	 * @param zx
	 * @param zy
	 * @param zz
	 */
	public Orientation(double xx, double xy, double xz, double yx, double yy, double yz, double zx, double zy,
			double zz) {
		super(xx, xy, xz, yx, yy, yz, zx, zy, zz);
	}

	/**
	 * @param values
	 */
	public Orientation(double values[][]) {
		super(values);
	}

	////////////////////////////////// Rotations around global (laboratory) axes

	/**
	 * @param axis  : index of the axis of rotation
	 * @param angle
	 * @return this = R . this (R the rotation matrix) Rotate the orientation around
	 *         one of the global (laboratory) axes
	 */
	public Orientation rotateAroundGlobalAxis(int axis, double angle) {
		rotationMatrix.setAxisAngle(axis, angle);
		this.multOtherThis(rotationMatrix);

		return this;
	}

	/**
	 * @param axis : index of the axis of rotation
	 * @param c    : cos(angle)
	 * @param s    : sin(angle)
	 * @return this = R . this (R the rotation matrix) Rotate the orientation around
	 *         one of the global (laboratory) axes
	 */
	public Orientation rotateAroundGlobalAxis(int axis, double c, double s) {
		rotationMatrix.setAxisAngle(axis, c, s);
		this.multOtherThis(rotationMatrix);
		applyGramSchmidtIfNecessary();

		return this;
	}

	/**
	 * @param axisVect coordinates of the axial vector in the global (laboratory)
	 *                 referential (does not need to have length one)
	 * @param angle
	 * @return this = R . this (R the rotation matrix) Rotate the orientation around
	 *         the axial vector with global (laboratory) coordinates axialVect
	 */
	public Orientation rotateAroundGlobalAxis(Vect axisVect, double angle) {
		rotationMatrix.setAxisAngle(axisVect, angle);
		this.multOtherThis(rotationMatrix);
		applyGramSchmidtIfNecessary();

		return this;
	}

	/**
	 * @param axisVect coordinates of the axial vector in the global (laboratory)
	 *                 referential (does not need to have length one)
	 * @param c        : cos(angle)
	 * @param s:       sin(angle)
	 * @return this = R . this (R the rotation matrix) Rotate the orientation around
	 *         the axial vector with global (laboratory) coordinates axialVect
	 */
	public Orientation rotateAroundGlobalAxis(Vect axisVect, double c, double s) {
		rotationMatrix.setAxisAngle(axisVect, c, s);
		this.multOtherThis(rotationMatrix);
		applyGramSchmidtIfNecessary();

		return this;
	}

	/**
	 * @param axesOrder : 0=>R0R1R2, 1=>R0R2R1, 2=>R1R0R2, 3=>R1R2R0, 4=>R2R0R1,
	 *                  5=>R2R1R0
	 * @param angle0    : angle for rotation around axis 0
	 * @param angle1    : angle for rotation around axis 1
	 * @param angle2    : angle for rotation around axis 2
	 * @return this = this . R (R the rotation matrix: R=R012, or R=R021, or ...)
	 *         Rotate orientation around 3 global axes in a given order.
	 */
	public Orientation rotateAroundGlobalAxes(int axesOrder, double angle0, double angle1, double angle2) {
		rotationMatrix.setAxesAngles(axesOrder, angle0, angle1, angle2);
		this.multOtherThis(rotationMatrix);
		applyGramSchmidtIfNecessary();

		return this;
	}

	/**
	 * @param axesOrder : 0=>R0R1R2, 1=>R0R2R1, 2=>R1R0R2, 3=>R1R2R0, 4=>R2R0R1,
	 *                  5=>R2R1R0
	 * @param c0        : cos of angle for rotation around axis 0
	 * @param c1        : cos of angle for rotation around axis 1
	 * @param c2        : cos of angle for rotation around axis 2
	 * @param s0        : sin of angle for rotation around axis 0
	 * @param s1        : sin of angle for rotation around axis 1
	 * @param s2        : sin of angle for rotation around axis 2
	 * @return this = this . R (R the rotation matrix: R=R012, or R=R021, or ...)
	 *         Rotate orientation around 3 global axes in a given order.
	 */
	public Orientation rotateAroundGlobalAxes(int axesOrder, double c0, double c1, double c2, double s0, double s1,
			double s2) {
		rotationMatrix.setAxesAngles(axesOrder, c0, c1, c2, s0, s1, s2);
		this.multOtherThis(rotationMatrix);
		applyGramSchmidtIfNecessary();

		return this;
	}

	////////////////////////////////// Rotations around local (body) axes

	/**
	 * @param axis  : index of the axis of rotation
	 * @param angle
	 * @return this = this . R (R the rotation matrix) Rotate the orientation around
	 *         one of its local (body) axes
	 */
	public Orientation rotateAroundLocalAxis(int axis, double angle) {
		rotationMatrix.setAxisAngle(axis, angle);
		this.mult(rotationMatrix);
		applyGramSchmidtIfNecessary();

		return this;
	}

	/**
	 * @param axis : index of the axis of rotation
	 * @param c    : cos(angle)
	 * @param s    : sin(angle)
	 * @return this = this . R (R the rotation matrix) Rotate the orientation around
	 *         one of its local (body) axes
	 */
	public Orientation rotateAroundLocalAxis(int axis, double c, double s) {
		rotationMatrix.setAxisAngle(axis, c, s);
		this.mult(rotationMatrix);
		applyGramSchmidtIfNecessary();

		return this;
	}

	/**
	 * @param axisVect coordinates of the axial vector in the local (body)
	 *                 referential (does not need to have length one)
	 * @param angle
	 * @return this = this . R (R the rotation matrix) Rotate the orientation around
	 *         the axial vector with local (body) coordinates axialVect
	 */
	public Orientation rotateAroundLocalAxis(Vect axisVect, double angle) {
		rotationMatrix.setAxisAngle(axisVect, angle);
		this.mult(rotationMatrix);
		applyGramSchmidtIfNecessary();

		return this;
	}

	/**
	 * @param axisVect coordinates of the axial vector in the local (body)
	 *                 referential (does not need to have length one)
	 * @param c        : cos(angle)
	 * @param s:       sin(angle)
	 * @return this = this . R (R the rotation matrix) Rotate the orientation around
	 *         the axial vector with local (body) coordinates axialVect
	 */
	public Orientation rotateAroundLocalAxis(Vect axisVect, double c, double s) {
		rotationMatrix.setAxisAngle(axisVect, c, s);
		this.mult(rotationMatrix);
		applyGramSchmidtIfNecessary();

		return this;
	}

	/**
	 * @param axesOrder : 0=>R0R1R2, 1=>R0R2R1, 2=>R1R0R2, 3=>R1R2R0, 4=>R2R0R1,
	 *                  5=>R2R1R0
	 * @param angle0    : angle for rotation around axis 0
	 * @param angle1    : angle for rotation around axis 1
	 * @param angle2    : angle for rotation around axis 2
	 * @return this = this . R (R the rotation matrix: R=R012, or R=R021, or ...)
	 *         Rotate orientation around 3 local axes in a given order.
	 */
	public Orientation rotateAroundLocalAxes(int axesOrder, double angle0, double angle1, double angle2) {
		rotationMatrix.setAxesAngles(axesOrder, angle0, angle1, angle2);
		this.mult(rotationMatrix);
		applyGramSchmidtIfNecessary();

		return this;
	}

	/**
	 * @param axesOrder : 0=>R0R1R2, 1=>R0R2R1, 2=>R1R0R2, 3=>R1R2R0, 4=>R2R0R1,
	 *                  5=>R2R1R0
	 * @param c0        : cos of angle for rotation around axis 0
	 * @param c1        : cos of angle for rotation around axis 1
	 * @param c2        : cos of angle for rotation around axis 2
	 * @param s0        : sin of angle for rotation around axis 0
	 * @param s1        : sin of angle for rotation around axis 1
	 * @param s2        : sin of angle for rotation around axis 2
	 * @return this = this . R (R the rotation matrix: R=R012, or R=R021, or ...)
	 *         Rotate orientation around 3 local axes in a given order.
	 */
	public Orientation rotateAroundLocalAxes(int axesOrder, double c0, double c1, double c2, double s0, double s1,
			double s2) {
		rotationMatrix.setAxesAngles(axesOrder, c0, c1, c2, s0, s1, s2);
		this.mult(rotationMatrix);
		applyGramSchmidtIfNecessary();

		return this;
	}

	/**
	 * Randomizing the orientation: 1) we reset the orientation to identity 2) we
	 * choose phi = 2 Pi u, theta = acos(2v-1) with u and v uniformly distributed
	 * between 0 and 1. See e.g.
	 * http://mathworld.wolfram.com/SpherePointPicking.html (note that unlike us
	 * they call the longitude theta and the latitude phi) 3) we chose a new
	 * direction of the local z axis by: rotating around the local z axis by phi
	 * rotating around the local y axis by theta 4) keeping the local z axis fixed
	 * we randomize the orientation of the 2 other local axes by rotating around the
	 * local z axis for an angle alpha uniformly distributed between 0 and 2Pi.
	 * 
	 * @return
	 */
	public Orientation randomize() {
		// first we reset the orientation to the lab coordinates
		this.setToIdentity();

		// now we chose a new direction (local z axis) at random
		// i.e. we pick randomly a point on the surface of a unit sphere
		double phi = RandomHelper.nextDoubleFromTo(0, 1) * 2 * Math.PI;
		double theta = Math.acos((2 * RandomHelper.nextDoubleFromTo(0, 1)) - 1);

		// we chose the new direction of the local z axis
		this.rotateAroundLocalAxis(2, phi);
		this.rotateAroundLocalAxis(1, theta);

		// keep local z-axis and randomize local x and y
		double alpha = RandomHelper.nextDoubleFromTo(0, 1) * 2 * Math.PI;
		this.rotateAroundLocalAxis(2, alpha);

		return this;
	}

	/**
	 * Counts the number of rotations since last Gram-Schmidt. If the rotationsCount
	 * is above rotationsCountThreshold, Gram-Schmidt is applied to the orientation.
	 */
	private void applyGramSchmidtIfNecessary() {
		rotationsCount++;

		if (!this.isOrthogonal()) {
			// TODO: generate an error (RotationMatrixNotOrthogonalError). This should be
			// done in RotationMatrix.
			System.out.println("WARNING: Orientation not orthogonal after " + rotationsCount + " rotations. "
					+ "Consider decreasing the rotationsCountThreshold. (See tests in OrientationTest.java)");
		}

		if (rotationsCount >= rotationsCountThreshold) {
			this.gramSchmidt();
			rotationsCount = 0;
		}
	}

	/**
	 * @return reference to the the 3rd column vector of the Orientation matrix Note
	 *         that further changes in the returned vector will be reflected in the
	 *         orientation matrix
	 */
	public Vect viewDirection() {
		return this.viewColumn(2);
	}

	public Matrix copy() {
		Orientation newO = new Orientation();
		newO.copy(this);

		// we don't care about copying the rotationMatrix.
		// it is always overwritten before it is used.
		// newO.rotationMatrix.copy(this.rotationMatrix);
		return newO;
	}

	/**
	 * @return
	 */
	public int getRotationsCount() {
		return rotationsCount;
	}

	/**
	 * @return
	 */
	public int getRotationsCountThreshold() {
		return rotationsCountThreshold;
	}

	/**
	 * @param i
	 */
	public void setRotationsCountThreshold(int i) {
		System.out.println("WARNING: you are changing the rotationsCountThreshold " + "from the default value, "
				+ this.getRotationsCountThreshold() + ",  to the new value " + i
				+ ". You should check that your orientation " + "matrix is still orthogonal.");
		rotationsCountThreshold = i;
	}

	public float[] getAxisAngle4f() {
		AxisAngle4f axAng = new AxisAngle4f();
		Matrix3f m = new Matrix3f(
				(float)this.getElement(0, 0), 
				(float)this.getElement(0, 1),
				(float)this.getElement(0, 2),
				(float)this.getElement(1, 0),
				(float)this.getElement(1, 1),
				(float)this.getElement(1, 2),
				(float)this.getElement(2, 0),
				(float)this.getElement(2, 1),
				(float)this.getElement(2, 2));
		axAng.set(m);
		return new float[] { axAng.x, axAng.y, axAng.z, axAng.angle };
	}
	/**
	 * This requires a pure rotation matrix 'm' as input. taken from:
	 * https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToAngle/index.htm
	 */
	public float[] toAxisAngle() {
		double angle, x, y, z; // variables for result
		double epsilon = 0.01; // margin to allow for rounding errors
		double epsilon2 = 0.1; // margin to distinguish between 0 and 180 degrees
		// optional check that input is pure rotation, 'isRotationMatrix' is defined at:
		// https://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/
		if ((Math.abs(rotationMatrix.getElement(0, 1) - rotationMatrix.getElement(1, 0)) < epsilon)
				&& (Math.abs(rotationMatrix.getElement(0, 2) - rotationMatrix.getElement(2, 0)) < epsilon)
				&& (Math.abs(rotationMatrix.getElement(1, 2) - rotationMatrix.getElement(2, 1)) < epsilon)) {
			// singularity found
			// first check for identity matrix which must have +1 for all terms
			// in leading diagonaland zero in other terms
			if ((Math.abs(rotationMatrix.getElement(0, 1) + rotationMatrix.getElement(1, 0)) < epsilon2)
					&& (Math.abs(rotationMatrix.getElement(0, 2) + rotationMatrix.getElement(2, 0)) < epsilon2)
					&& (Math.abs(rotationMatrix.getElement(1, 2) + rotationMatrix.getElement(2, 1)) < epsilon2)
					&& (Math.abs(rotationMatrix.getElement(0, 0) + rotationMatrix.getElement(1, 1)
							+ rotationMatrix.getElement(2, 2) - 3) < epsilon2)) {
				// this singularity is identity matrix so angle = 0
				return new float[] { 1, 0, 0, 0 }; // zero angle, arbitrary axis
			}
			// otherwise this singularity is angle = 180
			angle = Math.PI;
			double xx = (rotationMatrix.getElement(0, 0) + 1) / 2;
			double yy = (rotationMatrix.getElement(1, 1) + 1) / 2;
			double zz = (rotationMatrix.getElement(2, 2) + 1) / 2;
			double xy = (rotationMatrix.getElement(0, 1) + rotationMatrix.getElement(1, 0)) / 4;
			double xz = (rotationMatrix.getElement(0, 2) + rotationMatrix.getElement(2, 0)) / 4;
			double yz = (rotationMatrix.getElement(1, 2) + rotationMatrix.getElement(2, 1)) / 4;
			if ((xx > yy) && (xx > zz)) { // m[0][0] is the largest diagonal term
				if (xx < epsilon) {
					x = 0;
					y = 0.7071;
					z = 0.7071;
				} else {
					x = Math.sqrt(xx);
					y = xy / x;
					z = xz / x;
				}
			} else if (yy > zz) { // m[1][1] is the largest diagonal term
				if (yy < epsilon) {
					x = 0.7071;
					y = 0;
					z = 0.7071;
				} else {
					y = Math.sqrt(yy);
					x = xy / y;
					z = yz / y;
				}
			} else { // m[2][2] is the largest diagonal term so base result on this
				if (zz < epsilon) {
					x = 0.7071;
					y = 0.7071;
					z = 0;
				} else {
					z = Math.sqrt(zz);
					x = xz / z;
					y = yz / z;
				}
			}
			return new float[] { (float) x, (float) y, (float) z, (float) angle }; // return 180 deg rotation
		}
		// as we have reached here there are no singularities so we can handle normally
		double s = Math.sqrt((rotationMatrix.getElement(2, 1) - rotationMatrix.getElement(1, 2))
				* (rotationMatrix.getElement(2, 1) - rotationMatrix.getElement(1, 2))
				+ (rotationMatrix.getElement(0, 2) - rotationMatrix.getElement(2, 0))
						* (rotationMatrix.getElement(0, 2) - rotationMatrix.getElement(2, 0))
				+ (rotationMatrix.getElement(1, 0) - rotationMatrix.getElement(0, 1))
						* (rotationMatrix.getElement(1, 0) - rotationMatrix.getElement(0, 1))); // used to normalise
		if (Math.abs(s) < 0.001)
			s = 1;
		// prevent divide by zero, should not happen if matrix is orthogonal and should
		// be
		// caught by singularity test above, but I've left it in just in case
		angle = Math.acos((rotationMatrix.getElement(0, 0) + rotationMatrix.getElement(1, 1)
				+ rotationMatrix.getElement(2, 2) - 1) / 2);
		x = (rotationMatrix.getElement(2, 1) - rotationMatrix.getElement(1, 2)) / s;
		y = (rotationMatrix.getElement(0, 2) - rotationMatrix.getElement(2, 0)) / s;
		z = (rotationMatrix.getElement(1, 0) - rotationMatrix.getElement(0, 1)) / s;
		return new float[] { (float) x, (float) y, (float) z, (float) angle };
	}
}
