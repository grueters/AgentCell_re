public class UserMain {

	public UserMain() {};

	public void start() {
		String[] args = new String[] {
				"/home/leonm/git/AgentCell_re/AgentCell_re/AgentCell_re.rs" };
		repast.simphony.runtime.RepastMain.main(args);
	}

	public static void main(String[] args) {
		UserMain um = new UserMain();
		um.start();
	}
}
