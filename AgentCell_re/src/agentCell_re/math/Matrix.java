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


/**
 * @author emonet
 *
 * 
 * 
 */
public interface Matrix {
    /**
     * @param m
     * @return this = this + m
     * addition of two matrices and assignement of result to the caller
     */
    public Matrix plus(Matrix m);

    /**
     * @param m
     * @return this = this - m
     * Matrix substraction and assignement of result to the caller
     */
    public Matrix minus(Matrix m);

    /**
     * @param m
     * @return this = this * scalar
     *  scalar multiplication of a Matrix or and assignement of result to the caller
     */
    public Matrix mult(double scalar);

    /**
     * @param otherM
     * @return this = this . otherM
     *matrix multiplication and assignement to caller
     */
    public Matrix mult(Matrix otherM);

    /**
     * @param otherM
     * @return this = otherM . this
     *matrix multiplication and assignement to caller
     */
    public Matrix multOtherThis(Matrix otherM);

    /**
     * @param v
     * @return v = m.v
     * matrix vector multiplication. The vector is on the right.
     */
    public Vect mult(Vect v);

    /**
     * @param m
     * @return this = this / scalar
     *  division of a Matrix or by a scalar and assignement of result to the caller
     */
    public Matrix div(double scalar);

    /**
     * @return this
     * transpose this and return it
     */
    public Matrix transpose();

    /**
     * @return this
     * invert this if it is possible, or produces the pseudo-inverse
     * stores the result on this
     */
    public Matrix invert();

    /**
     * @return this
     * apply Gram-Schmidt on the matrix, i.e. make it orthogonal.
     * Work with columns, not with rows:
     * c0 = c0 / |c0|
     * c1 = c1 - (c1*c0) * c0
     * c1 = c1 / |c1|
     * c2 = c0 x c1
     */
    public Matrix gramSchmidt();

    /**
     * @return determinant of the caller
     */
    public double det();

    public int getRowSize();

    public int getColumnSize();

    public double getElement(int row, int column);

    public void setElement(int row, int column, double value);

    /**
     * @return newMatrix
     * return a new Matrix, copy of the caller
     */
    public Matrix copy();

    /**
     * @param m
     * @return this
     * copy the matrix m in the caller
     */
    public Matrix copy(Matrix m);

    /**
     * @param m
     * @return boolean
     * return true if this and m are the same object or
     * if the elements of this are equal to the elements of
     * v+-TOLERANCE as defined in Calculus.
     */
    public boolean equals(Object m);

    /**
     * @return boolean
     * returns true if this is the identity matrix with a tolerance as given by Calculus.TOLERENCE
     */
    public boolean isIdentity();

    /**
     * @return boolean
     * returns true if this is the zero matrix with a tolerance as given by Calculus.TOLERENCE
     */
    public boolean isZero();

    /**
     * @return this
     * maes this the identity matrix
     */
    public Matrix setToIdentity();

    /**
     * @return this
     * makes this the zero matrix
     */
    public Matrix setToZero();

    /*
     */
    public String toLog();

    /**
     * @return String
     * prints out the values of the vector as ((xx, xy, xz), (yx, yy, yz), (zx, zy, zz))
     */
    public String toString();

    /**
     * @param row
     * @return Vector reference to the wanted row
     * No new storage is created, instead a reference to the row is returned.
     * So future changes in the returned row vector will affect the calling matrix
     */
    public Vect viewRow(int row);

    /**
     * @param column
     * @return Vector reference to the wanted column
     * No new storage is created, instead a reference to the column is returned.
     * So future changes in the returned column vector will affect the calling matrix.
     */
    public Vect viewColumn(int column);

    /**
     * @param row
     * @return new Vector containing the wanted row
     * New storage is created.
     * So future changes in the returned row vector will NOT affect the calling matrix.
     */
    public Vect copyRow(int row);

    /**
     * @param column
     * @return new Vector containing the wanted column
     * New storage is created.
     * So future changes in the returned column vector will NOT affect the calling matrix.
     */
    public Vect copyColumn(int column);

    /**
     * @param row
     * @param rowVect a vector containing the values to be copied onto the row of this Matrix
     * @return this
     * Copies the values of the Vector argument rowVect onto the row of this Matrix.
     */
    public Matrix copyRow(int row, Vect rowVect);

    /**
     * @param column
     * @param columnVect a vector containing the values to be copied onto the column of this Matrix
     * @return this
     * Copies the values of the Vector argument columnVect onto the column of this Matrix.
     */
    public Matrix copyColumn(int column, Vect columnVect);

    /**
     * @return true if this is orthogonal, false if not.<br>
     * Test this matrix for orthogonality.
     * R orthogonal &hArr; R . transpose(R) = 1  &hArr;  columns are orthonormal &hArr; rows are orthonormal
     */
    public boolean isOrthogonal();
}
