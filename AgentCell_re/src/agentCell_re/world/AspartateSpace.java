package agentCell_re.world;

import agentCell_re.math.Vect;
import agentCell_re.math.Vect3;
import agentCell_re.models.AC_Parameters;
import repast.simphony.context.DefaultContext;
import repast.simphony.context.space.continuous.ContinuousSpaceFactoryFinder;
import repast.simphony.context.space.grid.GridFactoryFinder;
import repast.simphony.engine.environment.RunEnvironment;
import repast.simphony.parameter.Parameters;
import repast.simphony.space.continuous.ContinuousSpace;
import repast.simphony.space.continuous.SimpleCartesianAdder;
import repast.simphony.space.grid.Grid;
import repast.simphony.space.grid.GridBuilderParameters;
import repast.simphony.space.grid.RandomGridAdder;
import repast.simphony.space.grid.WrapAroundBorders;
import repast.simphony.valueLayer.GridValueLayer;

public class AspartateSpace extends DefaultContext<Object> {

	public AspartateSpace(AC_Parameters p, IWorld world) {
		super("AspartateSpace");
		
		int xdim = (int) p.getXdim();
		// int ydim = (int)p.getYdim();
		int ydim = (int) p.getZdim();

		Grid<Object> grid = GridFactoryFinder.createGridFactory(null).createGrid("Grid", this,
				new GridBuilderParameters<Object>(new WrapAroundBorders(), new RandomGridAdder<Object>(), true, xdim,
						ydim));
		
		GridValueLayer currentAspartate = new GridValueLayer("CurrentAspartate", true, new WrapAroundBorders(), xdim,
				ydim);
		/*
		 * GridValueLayer maxSugar = new GridValueLayer("MaxSugar", true, new
		 * WrapAroundBorders(), xdim, ydim);
		 */
		this.addValueLayer(currentAspartate);
		// this.addValueLayer(maxSugar);

		// Read in the sugar values from a file
		// PGMReader reader = new PGMReader(sugarFile);
		// int matrix[][] = reader.getMatrix();

		// create the sugar and fill the sugar space
		for (int x = 0; x < xdim; x++) {
			for (int y = 0; y < ydim; y++) {
				Vect3 position = new Vect3(x, 1, y);
				currentAspartate.set(world.getConcentrationField().getConcentrationLevelAt(position),x,y);
				// maxSugar.set(matrix[x][y], x,y);
			}
		}
	}

}
