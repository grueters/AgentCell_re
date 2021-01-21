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
 * Created on May 23, 2003
 *
 * 
 * 
 */
package agentCell_re.models;

import java.awt.Component;
import java.awt.Rectangle;
import java.awt.Window;
import java.io.File;
import java.util.Collection;
import java.util.Date;
import java.util.Iterator;

import org.apache.log4j.Logger;

import agentCell_re.cells.ChemotacticCell;
import agentCell_re.flagella.Flagella;
import agentCell_re.flagella.SwimmingFlagella;
import agentCell_re.math.Orientation;
import agentCell_re.math.Vect;
import agentCell_re.math.Vect3;
import agentCell_re.molecules.ConcentrationFieldConstantGradientWithBounds;
import agentCell_re.molecules.Copynumber;
import agentCell_re.molecules.Molecule;
import agentCell_re.motion.Run;
import agentCell_re.motion.RunTumbleStepper;
import agentCell_re.motion.TumbleGammaDistribution;
import agentCell_re.motor.AveragedCheYpThresholdMotor;
import agentCell_re.motor.Motor;
import agentCell_re.networks.ChemotaxisNetwork;
import agentCell_re.receptors.Receptors;
import agentCell_re.util.general.PathInterface;
import agentCell_re.util.hdf.ChemotaxisRecorder;
import agentCell_re.util.log4j.ClusterLogger;
import agentCell_re.world.AspartateIndicator;
import agentCell_re.world.AspartateSpace;
import agentCell_re.world.BoundaryConditions;
import agentCell_re.world.IWorld;
import agentCell_re.world.PeriodicBoundary;
import agentCell_re.world.ReflectiveBoundary;
import agentCell_re.world.World;
import repast.simphony.context.Context;
import repast.simphony.context.space.continuous.ContinuousSpaceFactoryFinder;
import repast.simphony.dataLoader.ContextBuilder;
import repast.simphony.engine.environment.RunEnvironment;
import repast.simphony.engine.schedule.ISchedule;
import repast.simphony.engine.schedule.ScheduleParameters;
import repast.simphony.parameter.Parameters;
import repast.simphony.random.RandomHelper;
import repast.simphony.space.continuous.ContinuousSpace;
import repast.simphony.space.continuous.SimpleCartesianAdder;
import repast.simphony.valueLayer.ContinuousValueLayer;

/**
 * @author north
 *
 *         All initializations of the model are done here. Here we choose if the
 *         cell has a Poisson motor, or use time series.
 */
/**
 * @author grueters This class follows
 *         https://repast.github.io/docs/RepastFAQ/RepastFAQ.html#_running_models
 */
public class ChemotaxisModel implements ContextBuilder<Object> {

	AC_Parameters acParams;

	private IWorld world;
	double aspartateMax = 1.0E-2;

	public Context build(Context<Object> context) {
		context.setId("agentCell_re");

		Parameters p = RunEnvironment.getInstance().getParameters();
		acParams = AC_Parameters.getInstance(p);
		double xdim = acParams.getXdim();
		double ydim = acParams.getYdim();
		double zdim = acParams.getZdim();

		ScheduleParameters scheduleParams = ScheduleParameters.createRepeating(1, acParams.getDT_s(), 0);
		RunEnvironment.getInstance().getCurrentSchedule().schedule(scheduleParams, this, "execute");

		long seed = new Date().getTime();
		RandomHelper.setSeed((int) seed);
		RandomHelper.createNormal(0.0, 1.0);
		RandomHelper.createGamma(1.0, 1.0);
		// double bla = RandomHelper.getGamma().nextDouble();

		ContinuousSpace<Object> space3d = ContinuousSpaceFactoryFinder.createContinuousSpaceFactory(null)
				.createContinuousSpace("space3d", context, new SimpleCartesianAdder<Object>(),
						new repast.simphony.space.continuous.BouncyBorders(), xdim, ydim, zdim);

		// Set the model step size to 0.01 seconds.
		this.setDt(acParams.getDT_s());

		// Have the model stop after stopTime seconds.
		this.setStopTime(acParams.getStopTime_s()); // 2000.0);
		double equilibrationTime = acParams.getEquilibrationTime_s(); // 1000.0;

		// Create a world.
		world = new World();

		// Add the world to the model.
		this.setWorld(world);

		// Let the world know about the model.
		world.setModel(this);

		// Create box boundary conditions.
		// like in Dahlquist, Lovely & Koshland, Nature new biol. 236, 120 (1972), Fig.
		// 4
		BoundaryConditions boundaryConditions = new BoundaryConditions(world);

		/*
		 * // x=-1 boundaryConditions.add( new ReflectiveBoundary(world, 1, 0, 0, -1, 0,
		 * 0)); // x=1 boundaryConditions.add( new ReflectiveBoundary(world, 99, 0, 0,
		 * 1, 0, 0)); // y=-1 boundaryConditions.add( new ReflectiveBoundary(world, 0,
		 * 1, 0, 0, -1, 0)); // y=1 boundaryConditions.add( new
		 * ReflectiveBoundary(world, 0, 99, 0, 0, 1, 0)); // z=-13 mm like in Dahlquist,
		 * Lovely & Koshland, Nature new biol. 236, 120 // (1972) //
		 * boundaryConditions.add(new ReflectiveBoundary(world, 0, 0, 0E3, 0, 0, -1));
		 * boundaryConditions.add(new ReflectiveBoundary(world, 0, 0, 1, 0, 0, -1)); //
		 * //use -3 (shorter)
		 * 
		 * // z= 32 mm (total length = 45, see fig 4) boundaryConditions.add( // new
		 * ReflectiveBoundary(world, 0, 0, 30E3, 0, 0, 1)); new
		 * ReflectiveBoundary(world, 0, 0, 199, 0, 0, 1));
		 */
		
		// margin=0.001
		// This is so that the Repast boundaries don't interfere with the AgentCell boundaries that we use.
		double margin = 0.001;
		double xMin, yMin, zMin;
		xMin = margin;
		yMin = margin; 
		zMin = margin; 
		double xMax = xdim - margin;
		double yMax = ydim - margin;
		double zMax = zdim - margin;
		
		//default period = 1
		double xPeriod = xMax - 0.001; // 99.999;
		double yPeriod = yMax - 0.001; // 99.999;
		double zPeriod = zMax - 0.001; // 199.999;
		
		// x=-1
		boundaryConditions.add(new PeriodicBoundary(world, xMin, 0, 0, -1, 0, 0, xPeriod));
		// x=1
		boundaryConditions.add(new PeriodicBoundary(world, xMax, 0, 0, 1, 0, 0, xPeriod));
		// y=-1
		boundaryConditions.add(new PeriodicBoundary(world, 0, yMin, 0, 0, -1, 0, yPeriod));
		// y=1
		boundaryConditions.add(new PeriodicBoundary(world, 0, yMax, 0, 0, 1, 0, yPeriod));
		
		/*
		 * // z=-13 mm like in Dahlquist, Lovely & Koshland, Nature new biol. 236, 120
		 * // (1972) // boundaryConditions.add(new ReflectiveBoundary(world, 0, 0, 0E3,
		 * 0, 0, -1)); boundaryConditions.add(new PeriodicBoundary(world, 0, 0, zMin, 0,
		 * 0, -1, zPeriod)); // //use -3 (shorter)
		 * 
		 * // z= 32 mm (total length = 45, see fig 4) boundaryConditions.add( // new
		 * ReflectiveBoundary(world, 0, 0, 30E3, 0, 0, 1)); new PeriodicBoundary(world,
		 * 0, 0, zMax, 0, 0, 1, zPeriod));
		 */
		
		// z=-13 mm like in Dahlquist, Lovely & Koshland, Nature new biol. 236, 120 
		// (1972) 
		// boundaryConditions.add(new ReflectiveBoundary(world, 0, 0, 0E3, 0, 0, -1));
		boundaryConditions.add(new ReflectiveBoundary(world, 0, 0, zMin, 0, 0, -1)); 
		//
		//use -3 (shorter)
		// z= 32 mm (total length = 45, see fig 4) 
		// boundaryConditions.add(new ReflectiveBoundary(world, 0, 0, 30E3, 0, 0, 1));
		boundaryConditions.add(new ReflectiveBoundary(world, 0, 0, zMax, 0, 0, 1));
		
		world.setBoundaryConditions(boundaryConditions);

		// Set the chemical gradient.
		double aspartateGradient = 1.0E-8; // M / micrometer
		world.setConcentrationField(new ConcentrationFieldConstantGradientWithBounds(Molecule.ASPARTATE, // type
				new Vect3(0, 0, 0), // origin
				0, // concentration at origin in Mol
				new Vect3(0, 0, aspartateGradient), 0, // min Level
				1.0E-2)); // max level

		/*
		 * AspartateSpace aspartateSpace = new AspartateSpace(acParams, world);
		 * context.addSubContext(aspartateSpace); 
		 * context.add(aspartateSpace);
		 */
		double xIndicator = xdim/2.0;
		double yIndicator = ydim/2.0;
		for (int z = 50; z < zdim; z += 100) {
			double zIndicator = (double)z;
			Vect v = new Vect3(xIndicator, yIndicator, zIndicator);
			double localAspartateLevel = world.getConcentrationField().getConcentrationLevelAt(v);
			AspartateIndicator aspIndicator = new AspartateIndicator(localAspartateLevel);
			context.add(aspIndicator);
			space3d.moveTo(aspIndicator, xIndicator, yIndicator, zIndicator);
		}
		
		for (int i = 0; i < acParams.getNumberOfCells(); i++) {

			// PARAMETER: initial position of the cell
			// double zpos = Random.uniform.nextDoubleFromTo(0,1) * 30000 - 3000;
			double xPos = RandomHelper.nextDoubleFromTo(xMin, xMax);
			double yPos = RandomHelper.nextDoubleFromTo(yMin, yMax);
			double zPos = (zdim / 2.0); //+ RandomHelper.nextDoubleFromTo(-5.0, 5.0);
			// double zpos = 100.0; // position chosen for the cell to be in 1 uM aspartate

			Vect position = new Vect3(xPos, yPos, zPos);
			// Create orientation for future cell
			// PARAMETER: initial orientation of the cell
			Orientation orientation = new Orientation(); // local and global axes are aligned

			// randomize orientation
			// orientation.randomize();

			// volume of a cell in Liters
			// PARAMETER: volume of the cell. Must be the same as in stochsim
			double cellVolume_l = acParams.getCellVolume_l();

			// create the cell
			// copy numbers are zero for the moment. Must be set after network is
			// initialized
			ChemotacticCell cell = new ChemotacticCell(space3d, world, position, orientation,
					new Copynumber(Molecule.CHEYP), cellVolume_l);
			context.add(cell);
			space3d.moveTo(cell, xPos, yPos, zPos);
			// set AbsolutePath of cell on filesytem
			// cell.setPath( cellArrayList.get(i).toString() );
			// cell.setPath(new File(cellArrayList.get(i).toString()).getAbsolutePath());
			String path = new File(acParams.getInputDirectory()).getAbsolutePath();
			cell.setPath(path);

			// set an equilibration time = initial period during which motion is suppress.
			// usefull to let the initial condition differentiate from the common ancestor.
			cell.setEquilibrationTime(acParams.getEquilibrationTime_s());

			// Create receptors (can also be commented to make cells insensitive)
			Receptors receptors = new Receptors(cell);

//          //steeplina case
//          receptors.getDissociationConstants().put("{asp0}",new Double(1.55779E-6)); 
//          receptors.getDissociationConstants().put("{asp1}",new Double(1.55779E-6)); 
//          receptors.getDissociationConstants().put("{asp2}",new Double(1.29032E-6)); 
//          receptors.getDissociationConstants().put("{asp3}",new Double(0.204081E-6)); 
//          receptors.getDissociationConstants().put("{asp4}",new Double(0.204081E-6)); 
//          receptors.getDissociationConstants().put("{asp0*}",new Double(90.0001E-6));  
//          receptors.getDissociationConstants().put("{asp1*}",new Double(90.0001E-6));  
//          receptors.getDissociationConstants().put("{asp2*}",new Double(3.15789E-6));  
//          receptors.getDissociationConstants().put("{asp3*}",new Double(2.58278E-6));  
//          receptors.getDissociationConstants().put("{asp4*}",new Double(2.58278E-6));  

//          //steeplina_08sense_sameActivity case
//          receptors.getDissociationConstants().put("{asp0}",new Double(1.63176E-6));
//          receptors.getDissociationConstants().put("{asp1}",new Double(1.63176E-6)); 
//          receptors.getDissociationConstants().put("{asp2}",new Double(1.39403E-6)); 
//          receptors.getDissociationConstants().put("{asp3}",new Double(0.290340E-6)); 
//          receptors.getDissociationConstants().put("{asp4}",new Double(0.290340E-6)); 
//          receptors.getDissociationConstants().put("{asp0*}",new Double(9.97985E-6));  
//          receptors.getDissociationConstants().put("{asp1*}",new Double(9.97985E-6));  
//          receptors.getDissociationConstants().put("{asp2*}",new Double(2.83987E-6));  
//          receptors.getDissociationConstants().put("{asp3*}",new Double(2.44326E-6));  
//          receptors.getDissociationConstants().put("{asp4*}",new Double(2.44326E-6));  

			receptors.getDissociationConstants().put("{asp}", new Double(1.71E-6));
			receptors.getDissociationConstants().put("{asp*}", new Double(12E-6));

			cell.setReceptors(receptors);

			// Create a Network

			// create and initialize network for the cell.
			// currently we are only supporting the Chemotaxis Stochsim Network, will be
			// relaxed in future and each cell will have a collecition of networks

			// Only as a comparison with the original "Example" code in AgentCell
			// cell.setChemotaxisNetwork(new ChemotaxisNetwork(cell, "STCHSTC.INI",
			// (new File(cell.getPath(), "network1") + File.separator + "Input")));

			cell.setChemotaxisNetwork(new ChemotaxisNetwork(cell, "STCHSTC.INI",
					(new File(cell.getPath(), "data") + File.separator + "Input")));
			// old: cell.setChemotaxisNetwork(new ChemotaxisNetwork(cell, "STCHSTC.INI",
			// (new File(cell.getPath(), "network1") + File.separator + "Input")));

			// Set the CheYp level in the cell at the same level as in stochsim
			cell.getChemotaxisNetwork().getCopynumber(cell.getCheYp());

			// If cell has receptors, make sure that receptors occupancy are same as in
			// StochSim
			if (cell.getReceptors() != null) {
				((ChemotaxisNetwork) cell.getChemotaxisNetwork()).updateReceptorsDynamicValues();
			}
			System.out.println("Initial level of Ligand = " + cell.getReceptors().getLigandConcentration() + " "
					+ cell.getReceptors().getLigandMolecularType());
			System.out.println("Initial receptors occupancies = ");
			Collection c = cell.getReceptors().getDissociationConstants().keySet();
			Iterator citer = c.iterator();
			while (citer.hasNext()) {
				String s = (String) citer.next();
				System.out.println("    " + s + " = " + cell.getReceptors().getOccupancy(s));
			}

			// Create a motor
			// Thresholds are chosen at a given percentage of sigma(Yp) + mean(Yp) of
			// equilibrated cell
			//
			// Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, 1420); //CHER=200
			// CWbias=0.22,0.17
			// Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, 1414); //CHER=200
			// CWbias=0.29,0.23
			// Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, 2039); //CHER=400
			// CWbias=0.29,0.23
			// Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, 2152); //CHER=800
			// CWbias=0.29,0.23
			//
			// Without Coupling steeplina case
			// Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, 1409); //0.337
			// sigma CHER=200 (WT)
			// Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, 2033); //0.3 sigma
			// CHER=400
			// Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, 2152); //0.3 sigma
			// CHER=800
			//
			// Hazelbauer rates
			// Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, 618); //CHER=034
			// CWbias=0.29,0.23
			// Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, 974); //CHER=068
			// CWbias=0.29,0.23
			Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, acParams.getCheYpThreshold()); // CHER=136
																										// CWbias=0.29,0.23
			// Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, 2889); //CHER=272
			// CWbias=0.29,0.23
			// Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, 3207); //CHER=544
			// CWbias=0.29,0.23
			// Copynumber cheYpThreshold = new Copynumber(Molecule.CHEYP, 3229); //CHEB=059
			// CWbias=0.29,0.23

			// initialMototState = CW if cell.CheYp > cheYpThreshold, = CCW else
			int initialMotorState;

			if (cell.getCheYp().getLevel() > cheYpThreshold.getLevel()) {
				initialMotorState = Motor.CW;
			} else {
				initialMotorState = Motor.CCW;
			}

			double minCWDuration = 0.1; // seconds
			double minCCWDuration = 0.1; // seconds
			double boxcarTimeWidth = acParams.getBoxcarTimeWidth_s(); // seconds
			cell.setMotor(new AveragedCheYpThresholdMotor(cell, initialMotorState, minCWDuration, minCCWDuration,
					cheYpThreshold, (int) Math.round(boxcarTimeWidth / acParams.getDT_s())));

			// cell.setMotor(new CheYpThresholdMotor(cell, initialMotorState,
			// minCWDuration,minCCWDuration,cheYpThreshold));
			// // Create a motorSeries that will have only 0s (run) events
			// ArrayList states = new ArrayList();
			// for (int j = 0; j < Math.round(model.getStopTime() / model.getDt())+2; j++) {
			// states.add(new Integer(0)); //only run
			// }
			// double minCWDuration=0.08; // seconds
			// double minCCWDuration=0.08; // seconds
			// cell.setMotor(new
			// SeriesThresholdMotor(cell,states,minCWDuration,minCCWDuration));
			// Create a flagella group
			int flagellaState = Flagella.INVALIDSTATE;

			if (cell.getMotor().getState() == Motor.CCW) {
				flagellaState = Flagella.BUNDLED;
			} else {
				flagellaState = Flagella.APART;
			}

			// cell.setFlagella(new TetheredFlagellum(cell,flagellaState));
			cell.setFlagella(new SwimmingFlagella(cell, flagellaState));

			// Create a motion stepper with run and tumble states.
			RunTumbleStepper motionStepper = new RunTumbleStepper(cell);

			// Motion:
			// PARAMETERS:
			// speed = 20 [um/sec]
			// sphere radius=1 [um = 1E-6 m]
			// T=305 [K]
			// viscosity = 0.027 [g/(cm sec)]
			// boltzman = 1.3807E-16 [erg/deg (= g cm^2/sec^2)]
			// rotationaDiffusion = boltzman * T / (8*PI*viscosity*(1e-4*radius)^3) =
			// 0.0620577 [sec^-1]
			double runSpeed = acParams.getCellSpeed_microm_per_s();
			double runRotationalDiffusion = 0.0620577;
			motionStepper.setRun(new Run(motionStepper, runSpeed, runRotationalDiffusion));

			// Tumble that fits the distribution in
			// Fig. 3, p. 501 from Berg and Brown, Nature, 239, 500 (1972)
			double shape = 4;
			double scale = 18.32045567939674;
			double location = -4.606176605609249;
			motionStepper.setTumble(new TumbleGammaDistribution(motionStepper, shape, scale, location));

			// motionStepper.setTumble(new TumbleCosinusDistribution(motionStepper));
			cell.setMotionStepper(motionStepper);

			// print info about cell
			System.out.println("Initial position of cell " + cell.getIdentifier() + " is :" + cell.getPosition());

			// Add the new cell to the world.
			world.getCells().add(cell);
		}
		
		// We need to obtain the CellDisplay-Window from Repast here...
		Window activeWindow = javax.swing.FocusManager.getCurrentManager().getActiveWindow();
		
		DisplayStandardizer.initialize(activeWindow);

		if (RunEnvironment.getInstance().isBatch()) {
			RunEnvironment.getInstance().endAt(20);
		}

		return context;   
	}

	private void fillEnvironmentalSpace(ContinuousValueLayer currentEnv) {
		int _xdim = (int) acParams.getXdim();
		int _ydim = (int) acParams.getYdim();
		int _zdim = (int) acParams.getZdim();
		for (int z = 0; z <= _zdim; z++) {
			for (int x = 0; x <= _xdim; x++) {
				for (int y = 0; y <= _ydim; y++) {
					double val = (double) z / _zdim;
					val *= aspartateMax;
					currentEnv.set(val, x, y, z);
				}
			}
		}
	}

	public void execute() {
		if (this.getSchedule().getTickCount() == 0.0) {
			this.begin();
		}
		// this.log();
		if (this.getSchedule().getTickCount() == acParams.getStopTime_s()) {
			this.end();
		}
	}

	public ISchedule getSchedule() {
		return RunEnvironment.getInstance().getCurrentSchedule();
	}

	public AC_Parameters getAcParams() {
		return acParams;
	}

	// Declarations of variables NO FILINING IN VALUES OTHER THAN DEFAULTS

	// TODO: remove these strings and replace with File ioPath;
	public static String fileSeparator = File.separator;
	public static String lineSeparator = System.getProperty("line.separator", "//");
	public static String runDirRelativePath = System.getProperty("runDirRelativePath", "1").trim();
	public static String runDirAbsolutePath = new File(runDirRelativePath).getAbsolutePath();
	// File ioPath;

	// Declare the main loggers
	public static Logger log4jLogger = null;
	private double dt = Double.NaN;
	private long nsteps;
	// Declare the startup routine.
	private double stopTime = Double.NaN;

	// TODO: remove
	// public String agentcellNumberOfCells;

	/*
	 * public void setup() { isGui = !(getController().isBatch()); agentList = new
	 * ArrayList(); }
	 */

	/**
	 * Sets the random number seed for this model, and recreates a uniform
	 * distribution with that seed.
	 */
	public void setRngSeed(long seed) {
	}

	public void generateNewSeed() {
	}

	// Declare the startup routine.
	public void begin() {

		// create the logger (log4j) ???
		ChemotaxisModel.log4jLogger = ClusterLogger.getMainLoggerCategory("RUNLOG");

		// assuming nsteps can't change per run so I'll make it visible through the
		// model
		this.nsteps = Math.round(this.getStopTime() / this.getDt());
		if (this.nsteps > Integer.MAX_VALUE) {
			System.err.println("Too many time steps. To handle that many time steps \n"
					+ "you must refactor ChemotaxisRecorder to use a long for maxTimeSteps");
			System.err.flush();
			System.exit(1);
		}
	}

	public void end() {
		System.out.println("Done!");

		// close the output file associated with each cell
		Iterator it = this.getWorld().getCells().iterator();

		while (it.hasNext()) {
			ChemotacticCell cell = (ChemotacticCell) it.next();
			cell.getHdfLogger().close();
		}

	}

	// TODO: loggers should be defined when one built the cell and decide what to
	// log. Thus this should be moved to the place where one creates the cells from
	// the XML file. At that point, one knows what to log.
	/*
	 * log, logs both the log4j log capturing output for run, in runs/runX, as
	 * /run/runX/runX.OUT Also, creates/writes hdf output for each cell.
	 */
	public void log() {
		Iterator it = this.getWorld().getCells().iterator();

		while (it.hasNext()) {

			ChemotacticCell cell = (ChemotacticCell) it.next();

			// take care of the case when there is no receptors
			double receptorsOccupancy = 0;
			double receptorsOccupancyActive = 0;
			double ligand = 0;

			if (cell.getReceptors() != null) {
				receptorsOccupancy = cell.getReceptors().getOccupancy("{asp}");
				receptorsOccupancyActive = cell.getReceptors().getOccupancy("{asp*}");
//	                receptorsOccupancy = cell.getReceptors().getOccupancy("{asp2}");
//	                receptorsOccupancyActive = cell.getReceptors().getOccupancy("{asp2*}");
				ligand = cell.getReceptors().getLigandConcentration();
			}

			// take care of the case when there is no flagella
			int flagellaState = 0;

			if (cell.getFlagella() != null) {
				flagellaState = cell.getFlagella().getState();
			}

			ChemotaxisModel.log4jLogger.info("" + ((float) this.getSchedule().getTickCount()) + ","
					+ cell.getIdentifier() + "," + cell.getPosition().toLog() + "," + cell.getOrientation().toLog()
					+ "," + cell.getMotor().getState() + "," + cell.getCheYp().getLevel() + ","
					+ ((float) receptorsOccupancy) + "," + ((float) ligand) + "," + flagellaState + ","
					+ ((float) receptorsOccupancyActive));

			if (cell.getHdfLogger() == null) {
//	            	String hdfLoc = cell.getPath() + PathInterface.fileSeparator + "network1" + PathInterface.fileSeparator + 
//	                			"Output" + PathInterface.fileSeparator + "CELL" +  cell.getIdentifier() + ".hdf";
//	            	System.out.println("ChemotaxisModel; hdf4 file:  " + hdfLoc);

				cell.setHdfLogger(new ChemotaxisRecorder(cell.getPath() + PathInterface.fileSeparator + "CELL" + ".hdf",
						4, (int) nsteps));
			}

			cell.getHdfLogger().getX().record((float) cell.getPosition().getElement(0));
			cell.getHdfLogger().getY().record((float) cell.getPosition().getElement(1));
			cell.getHdfLogger().getZ().record((float) cell.getPosition().getElement(2));
			cell.getHdfLogger().getXx().record((float) cell.getOrientation().getElement(0, 0));
			cell.getHdfLogger().getXy().record((float) cell.getOrientation().getElement(0, 1));
			cell.getHdfLogger().getXz().record((float) cell.getOrientation().getElement(0, 2));
			cell.getHdfLogger().getYx().record((float) cell.getOrientation().getElement(1, 0));
			cell.getHdfLogger().getYy().record((float) cell.getOrientation().getElement(1, 1));
			cell.getHdfLogger().getYz().record((float) cell.getOrientation().getElement(1, 2));
			cell.getHdfLogger().getZx().record((float) cell.getOrientation().getElement(2, 0));
			cell.getHdfLogger().getZy().record((float) cell.getOrientation().getElement(2, 1));
			cell.getHdfLogger().getZz().record((float) cell.getOrientation().getElement(2, 2));

			cell.getHdfLogger().getCheYp().record((int) cell.getCheYp().getLevel());
			cell.getHdfLogger().getMotorState().record((int) cell.getMotor().getState());

			cell.getHdfLogger().getReceptorsOccupancy().record((float) receptorsOccupancy);
			cell.getHdfLogger().getLigand().record((float) ligand);
			cell.getHdfLogger().getFlagellaState().record(flagellaState);
			cell.getHdfLogger().getReceptorsOccupancyActive().record((float) receptorsOccupancyActive);

			// //Tests output
			// System.out.println(
			// cell.getMotor().getState()+" "+
			// cell.getFlagella().getState()+" "+
			// cell.getOrientation().getElement(0,2)+" "+
			// cell.getOrientation().getElement(1,2)+" "+
			// cell.getOrientation().getElement(2,2)+" ");
		}
	}
	// TODO: ALL THE WAY UP TO HERE

	/**
	 * @return
	 */
	public String[] getInitParam() {
		return new String[] { "stopTime" };
	} // public static ChemotaxisRecorder hdfLogger = null;

	/**
	 * @return
	 */
	public IWorld getWorld() {
		return world;
	}

	/**
	 * @param world
	 */
	public void setWorld(IWorld world) {
		this.world = world;
	}

	/**
	 * @return
	 */
	public double getStopTime() {
		return stopTime;
	}

	/**
	 * @param d
	 */
	public void setStopTime(double d) {
		stopTime = d;
	}

	/**
	 * @return
	 */
	public double getDt() {
		return dt;
	}

	/**
	 * @param d
	 */
	public void setDt(double d) {
		dt = d;
	}

}
