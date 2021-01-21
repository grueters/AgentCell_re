package agentCell_re.world;

import agentCell_re.math.Vect3;

public class AspartateLevelIndicator {
	IWorld world;
	double XPosition;
	double YPosition;
	double ZPosition;
	double AspartateLevel;
	public AspartateLevelIndicator(World world, double XPosition, double YPosition, double ZPosition) {
		this.XPosition = XPosition;
		this.YPosition = YPosition;
		this.ZPosition = ZPosition;
		Vect3 vect = new Vect3(XPosition, YPosition, ZPosition);
		this.AspartateLevel = world.getConcentrationField().getConcentrationLevelAt(vect);
	}
}
