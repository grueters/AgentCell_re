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

import cern.colt.matrix.DoubleMatrix1D;
import cern.colt.matrix.DoubleMatrix2D;
import cern.colt.matrix.impl.DenseDoubleMatrix2D;
import cern.colt.matrix.linalg.Algebra;

import cern.jet.math.Functions;

import corejava.Format;


/**
 * @author emonet
 *
 * Implements Matrix using the colt library.
 * The matrices are 3x3 matrices of doubles with methods for the typical vector and matrix analysis.
 */
public class Matrix3x3 implements Matrix {
    protected static final int DIMENSION = 3;
    protected static final Algebra ALGEBRA = new Algebra(Calculus.TOLERANCE);
    protected static final Format FORMAT = new Format("%1.2f");
    protected DoubleMatrix2D data = new DenseDoubleMatrix2D(DIMENSION, DIMENSION);

    public Matrix3x3() {
    }

    public Matrix3x3(double xx, double xy, double xz, double yx, double yy,
        double yz, double zx, double zy, double zz) {
        data.setQuick(0, 0, xx);
        data.setQuick(0, 1, xy);
        data.setQuick(0, 2, xz);
        data.setQuick(1, 0, yx);
        data.setQuick(1, 1, yy);
        data.setQuick(1, 2, yz);
        data.setQuick(2, 0, zx);
        data.setQuick(2, 1, zy);
        data.setQuick(2, 2, zz);
    }

    public Matrix3x3(double values[][]) {
        data.assign(values);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#div(double)
     */
    public Matrix div(double scalar) {
        data.assign(Functions.div(scalar));

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#gramSchmidt()
     */
    public Matrix gramSchmidt() {
        //System.out.println("Applying Gram-Schmidt on this:" + this);
        DoubleMatrix1D v0 = data.viewColumn(0);
        DoubleMatrix1D v1 = data.viewColumn(1);
        DoubleMatrix1D v2 = data.viewColumn(2);

        // v0 = v0 / |v0|
        double len = Math.sqrt(ALGEBRA.norm2(v0));
        v0.assign(Functions.div(len));

        // len = v1 . v0
        len = v1.zDotProduct(v0);

        //v1 = v1 - len * v0
        v1.assign(v0, Functions.minusMult(len));

        //v1 = v1 / |v1|
        len = Math.sqrt(ALGEBRA.norm2(v1));
        v1.assign(Functions.div(len));

        //v2 = v0 x v1
        v2.setQuick(0,
            (v0.getQuick(1) * v1.getQuick(2)) -
            (v0.getQuick(2) * v1.getQuick(1)));
        v2.setQuick(1,
            (v0.getQuick(2) * v1.getQuick(0)) -
            (v0.getQuick(0) * v1.getQuick(2)));
        v2.setQuick(2,
            (v0.getQuick(0) * v1.getQuick(1)) -
            (v0.getQuick(1) * v1.getQuick(0)));

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#minus(edu.uchicago.agentcell.math.Matrix)
     */
    public Matrix minus(Matrix m) {
        data.assign(((Matrix3x3) m).data, Functions.minus);

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#mult(double)
     */
    public Matrix mult(double scalar) {
        data.assign(Functions.mult(scalar));

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#mult(edu.uchicago.agentcell.math.Matrix)
     */
    public Matrix mult(Matrix otherM) {
        data.copy().zMult(((Matrix3x3) otherM).data, data);

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#mult(edu.uchicago.agentcell.math.Vect)
     */
    public Vect mult(Vect v) {
        DoubleMatrix1D vData = ((Vect3) v).getData();
        data.zMult(vData.copy(), vData);

        return v;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#plus(edu.uchicago.agentcell.math.Matrix)
     */
    public Matrix plus(Matrix m) {
        data.assign(((Matrix3x3) m).data, Functions.plus);

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#transpose()
     */
    public Matrix transpose() {
        data = data.viewDice();

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#copy()
     */
    public Matrix copy() {
        Matrix3x3 newMatrix = new Matrix3x3();
        newMatrix.data.assign(data);

        return newMatrix;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#copy(edu.uchicago.agentcell.math.Matrix)
     */
    public Matrix copy(Matrix m) {
        data.assign(((Matrix3x3) m).data);

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#getColumnSize()
     */
    public int getColumnSize() {
        return data.columns();
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#getElement(int, int)
     */
    public double getElement(int row, int column) {
        return data.getQuick(row, column);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#getRowSize()
     */
    public int getRowSize() {
        return data.rows();
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#setElement(int, int, double)
     */
    public void setElement(int row, int column, double value) {
        data.setQuick(row, column, value);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#det()
     */
    public double det() {
        return ALGEBRA.det(data);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#invert()
     */
    public Matrix invert() {
        data = ALGEBRA.inverse(data);

        return this;
    }

    /**
     * @return data
     * get the the implemented data matrix. Should be used ONLY by the classes implementing Vector, Matrix, ...
     */
    DoubleMatrix2D getData() {
        return data;
    }

    /* (non-Javadoc)
     * @see java.lang.Object#equals(java.lang.Object)
     */
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }

        // using colt equals with correct tolerance
        if ((obj != null) && getClass().equals(obj.getClass()) &&
                ALGEBRA.property().equals(data, ((Matrix3x3) obj).data)) {
            return true;
        }

        return false;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#isIdentity()
     */
    public boolean isIdentity() {
        return ALGEBRA.property().isIdentity(data);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#isZero()
     */
    public boolean isZero() {
        return ALGEBRA.property().isZero(data);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#copyColumn(int, edu.uchicago.agentcell.math.Vect)
     */
    public Matrix copyColumn(int column, Vect columnVect) {
        data.viewColumn(column).assign(((Vect3) columnVect).getData());

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#copyColumn(int)
     */
    public Vect copyColumn(int column) {
        Vect3 columnVect = new Vect3();
        columnVect.getData().assign(data.viewColumn(column));

        return columnVect;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#copyRow(int, edu.uchicago.agentcell.math.Vect)
     */
    public Matrix copyRow(int row, Vect rowVect) {
        data.viewRow(row).assign(((Vect3) rowVect).getData());

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#copyRow(int)
     */
    public Vect copyRow(int row) {
        Vect3 rowVect = new Vect3();
        rowVect.getData().assign(data.viewRow(row));

        return rowVect;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#getColumn(int)
     * Only a new object is created, but no new data storage space.
     */
    public Vect viewColumn(int column) {
        return new Vect3(data.viewColumn(column));
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#getRow(int)
     * Only a new object is created, but no new data storage space.
     */
    public Vect viewRow(int row) {
        return new Vect3(data.viewRow(row));
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#multOtherThis(edu.uchicago.agentcell.math.Matrix)
     */
    public Matrix multOtherThis(Matrix otherM) {
        ((Matrix3x3) otherM).data.zMult(data.copy(), data);

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#setToIdentity()
     */
    public Matrix setToIdentity() {
        data.assign(0);
        data.setQuick(0, 0, 1);
        data.setQuick(1, 1, 1);
        data.setQuick(2, 2, 1);

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#setToZero()
     */
    public Matrix setToZero() {
        data.assign(0);

        return this;
    }

    /*
     */
    public String toLog() {
        return "" + ((float) data.getQuick(0, 0)) + "," +
        ((float) data.getQuick(0, 1)) + "," + ((float) data.getQuick(0, 2)) +
        "," + "" + ((float) data.getQuick(1, 0)) + "," +
        ((float) data.getQuick(1, 1)) + "," + ((float) data.getQuick(1, 2)) +
        "," + ((float) data.getQuick(2, 0)) + "," +
        ((float) data.getQuick(2, 1)) + "," + ((float) data.getQuick(2, 2));
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#toString()
     * uses FORMAT to do it. FORMAT is a static final attribute of Matrix3x3.
     */
    public String toString() {
        return "((" + FORMAT.format(data.getQuick(0, 0)) + ", " +
        FORMAT.format(data.getQuick(0, 1)) + ", " +
        FORMAT.format(data.getQuick(0, 2)) + "), " + "(" +
        FORMAT.format(data.getQuick(1, 0)) + ", " +
        FORMAT.format(data.getQuick(1, 1)) + ", " +
        FORMAT.format(data.getQuick(1, 2)) + "), " + "(" +
        FORMAT.format(data.getQuick(2, 0)) + ", " +
        FORMAT.format(data.getQuick(2, 1)) + ", " +
        FORMAT.format(data.getQuick(2, 2)) + "))";
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Matrix#isOrthogonal()
     */
    public boolean isOrthogonal() {
        return ALGEBRA.property().isOrthogonal(this.data);
    }
}
