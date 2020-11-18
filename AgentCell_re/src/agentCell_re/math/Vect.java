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
 * Created on Feb 23, 2004
 *
 * 
 * 
 */
package agentCell_re.math;


/**
 * @author Thierry Emonet
 *
 * Abstract class that defines the geometric vectors and their arythmetics.
 */
public interface Vect {
    /**
     * @param v
     * @return this = this + v
     * addition of two vectors and assignement of result to the caller
     */
    public Vect plus(Vect v);

    /**
     * @param scalar
     * @param v
     * @return this = this + scalar * v
     * addition of a vector times a scalar
     */
    public Vect plusMult(double scalar, Vect v);

    /**
     * @param v
     * @return this = this - v
     * vector substraction and assignement of result to the caller
     */
    public Vect minus(Vect v);

    /**
     * @param scalar
     * @param v
     * @return this = this - scalar * v
     * substraction of a vector times a scalar
     */
    public Vect minusMult(double scalar, Vect v);

    /**
     * @param v
     * @return this = this * scalar
     *  scalar multiplication of a vector and assignement of result to the caller
     */
    public Vect mult(double scalar);

    /**
     * @param v
     * @return this = this . v
     * dot product between two vectors
     */
    public double mult(Vect v);

    /**
     * @param m
     * @return this = this . m
     * multiplication of a vector and a matrix (vector is on the left)
     */
    public Vect mult(Matrix m);

    /**
     * @param v
     * @return this = this / scalar
     *  division of a vector by a scalar and assignement of result to the caller
     */
    public Vect div(double scalar);

    /**
     * @param otherV
     * @return this = this cross otherV
     * cross product between two vectors and assignement of result to the caller
     */
    public Vect cross(Vect otherV);

    /**
     * @param otherV
     * @return this = otherV cross this
     * cross product between two vectors and assignement of result to the caller
     */
    public Vect crossOtherThis(Vect otherV);

    /**
     * @return length of a vector
     */
    public double length();

    /**
     * Reverse the direction of the vector. I.e. multiply the vector by -1 and assign the result to the caller.
     */
    public Vect reverse();

    /**
     * Normalizes the vector. Divides the vector by its length
     */
    public Vect normalize();

    /**
     * @return the DIMENSION of the vector
     */
    public int getSize();

    /**
     * @param index
     * @return value
     * get the value of the index-th element of the vector
     */
    public double getElement(int index);

    /**
     * @param index
     * @param value
     * set the value of one of the elements
     */
    public void setElement(int index, double value);

    /**
     * @return newVect
     * return a new Vect, copy of the caller
     */
    public Vect copy();

    /**
     * @param v
     * @return this
     * copy v into the caller
     */
    public Vect copy(Vect v);

    /**
     * @param v
     * @return boolean
     * return true if this and v are the same object or
     * if the elements of this are equal to the elements of
     * v+-TOLERANCE as defined in Calculus.
     */
    public boolean equals(Object v);

    /**
     * @return boolean
     * returns true if the length of the vector is 1+-TOLERANCE as defined in Calculus
     */
    public boolean hasUnitLength();

    /**
     * @return boolean
     * returns true if all the elements of the vector are
     * smaller than the TOLERANCE defined in Calculus.
     */
    public boolean isZero();

    /*
     */
    public String toLog();

    /**
     * @return String
     * prints out the values of the vector as (x, y, z)
     */
    public String toString();
}
