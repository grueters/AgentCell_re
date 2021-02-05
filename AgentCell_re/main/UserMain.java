import java.io.File;

public class UserMain {

	public UserMain() {};

	public void start() {
		String userDir = System.getProperty("user.dir");
		String path = userDir + File.separator + "AgentCell_re.rs";
		String[] args = new String[] { path };
		repast.simphony.runtime.RepastMain.main(args);
	}

	public static void main(String[] args) {
		UserMain um = new UserMain();
		um.start();
	}
}
