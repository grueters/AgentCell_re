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
import cern.colt.matrix.impl.DenseDoubleMatrix1D;
import cern.colt.matrix.linalg.Algebra;

import cern.jet.math.Functions;

import corejava.Format;


/**
 * @author emonet
 *
 * Implements Vect using the colt library.
 * The Vect3 are 3D vectors with methods for the typical vector analysis.
 */
public class Vect3 implements Vect {
    private static final int DIMENSION = 3;
    private static final Algebra coltAlgebra = new Algebra(Calculus.TOLERANCE);
    protected static final Format FORMAT = new Format("%1.2f");
    private DoubleMatrix1D data;

    /**
     * Default constructor for Vect3. Creates a Vect3 zero vector = (0,0,0).
     */
    public Vect3() {
        data = new DenseDoubleMatrix1D(DIMENSION);
    }

    /**
     * @param x
     * @param y
     * @param z
     * Constructor for Vect3. Creates a Vect3 vector = (x,y,z).
     */
    public Vect3(double x, double y, double z) {
        data = new DenseDoubleMatrix1D(DIMENSION);
        data.setQuick(0, x);
        data.setQuick(1, y);
        data.setQuick(2, z);
    }

    /**
     * @param newData
     * Vect3 constructor used only within the package.
     * The argument is a DoubleMatrix1D from the colt library.
     * This allows to create Vect3 with data pointing somewhere else, e.g. a row of a Matrix3x3 object.
     */
    Vect3(DoubleMatrix1D newData) {
        data = newData;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#cross(edu.uchicago.agentcell.math.Vect)
     */
    public Vect cross(Vect otherV) {
        Vect3 v = (Vect3) otherV;
        double a = (data.getQuick(1) * v.data.getQuick(2)) -
            (data.getQuick(2) * v.data.getQuick(1));
        double b = (data.getQuick(2) * v.data.getQuick(0)) -
            (data.getQuick(0) * v.data.getQuick(2));
        data.setQuick(2,
            (data.getQuick(0) * v.data.getQuick(1)) -
            (data.getQuick(1) * v.data.getQuick(0)));
        data.setQuick(1, b);
        data.setQuick(0, a);

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#div(double)
     */
    public Vect div(double scalar) {
        data.assign(Functions.div(scalar));

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#length()
     */
    public double length() {
        return Math.sqrt(coltAlgebra.norm2(data));
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#minus(edu.uchicago.agentcell.math.Vect)
     */
    public Vect minus(Vect v) {
        data.assign(((Vect3) v).data, Functions.minus);

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#mult(double)
     */
    public Vect mult(double scalar) {
        data.assign(Functions.mult(scalar));

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#mult(edu.uchicago.agentcell.math.Vect)
     */
    public double mult(Vect v) {
        return data.zDotProduct(((Vect3) v).data);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#normalize()
     */
    public Vect normalize() {
        data.assign(Functions.div(Math.sqrt(data.zDotProduct(data))));

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#plus(edu.uchicago.agentcell.math.Vect)
     */
    public Vect plus(Vect v) {
        data.assign(((Vect3) v).data, Functions.plus);

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#reverse()
     */
    public Vect reverse() {
        data.assign(Functions.mult(-1));

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#copy()
     */
    public Vect copy() {
        return new Vect3(data.getQuick(0), data.getQuick(1), data.getQuick(2));
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#copy(edu.uchicago.agentcell.math.Vect)
     */
    public Vect copy(Vect v) {
        data.setQuick(0, ((Vect3) v).data.getQuick(0));
        data.setQuick(1, ((Vect3) v).data.getQuick(1));
        data.setQuick(2, ((Vect3) v).data.getQuick(2));

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#getElement(int)
     */
    public double getElement(int index) {
        return data.getQuick(index);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#getSize()
     */
    public int getSize() {
        return DIMENSION;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#setElement(int, double)
     */
    public void setElement(int index, double value) {
        data.setQuick(index, value);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#mult(edu.uchicago.agentcell.math.Matrix)
     * Assuming this is a column vector, we are doing: Transpose(this) x M = Transpose(M) x this
     */
    public Vect mult(Matrix m) {
        ((Matrix3x3) m).getData().zMult(data.copy(), data, 1, 0, true);

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#isUnit()
     */
    public boolean hasUnitLength() {
        return Calculus.equals(this.length(), 1);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#isZero()
     */
    public boolean isZero() {
        return Calculus.equals(data.getQuick(0), 0) &&
        Calculus.equals(data.getQuick(1), 0) &&
        Calculus.equals(data.getQuick(2), 0);
    }

    /* (non-Javadoc)
     * @see java.lang.Object#equals(java.lang.Object)
     */
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }

        if ((obj != null) && getClass().equals(obj.getClass()) &&
                coltAlgebra.property().equals(data, ((Vect3) obj).data)) {
            // using colt equals with correct tolerance
            return true;
        }

        return false;
    }

    /**
     * @return data
     * get the the implemented data vector. Should be used ONLY by the classes implementing Matrix, Vect, ...
     */
    DoubleMatrix1D getData() {
        return data;
    }

    /*
     */
    public String toLog() {
        return "" + ((float) data.getQuick(0)) + "," +
        ((float) data.getQuick(1)) + "," + ((float) data.getQuick(2));
    }

    /* (non-Javadoc)
     * @see java.lang.Object#toString()
     */
    public String toString() {
        return "(" + FORMAT.format(data.getQuick(0)) + ", " +
        FORMAT.format(data.getQuick(1)) + ", " + FORMAT.format(data.getQuick(2)) +
        ")";
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#crossOtherThis(edu.uchicago.agentcell.math.Vect)
     */
    public Vect crossOtherThis(Vect otherV) {
        Vect3 v = (Vect3) otherV;
        double a = (data.getQuick(2) * v.data.getQuick(1)) -
            (data.getQuick(1) * v.data.getQuick(2));
        double b = (data.getQuick(0) * v.data.getQuick(2)) -
            (data.getQuick(2) * v.data.getQuick(0));
        data.setQuick(2,
            (data.getQuick(1) * v.data.getQuick(0)) -
            (data.getQuick(0) * v.data.getQuick(1)));
        data.setQuick(1, b);
        data.setQuick(0, a);

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#minusMult(double, edu.uchicago.agentcell.math.Vect)
     */
    public Vect minusMult(double scalar, Vect v) {
        data.assign(((Vect3) v).data, Functions.minusMult(scalar));

        return this;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.Vect#plusMult(double, edu.uchicago.agentcell.math.Vect)
     */
    public Vect plusMult(double scalar, Vect v) {
        data.assign(((Vect3) v).data, Functions.plusMult(scalar));

        return this;
    }
}
