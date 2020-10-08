import java.util.*;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.locks.ReentrantLock;


/**
 * Class that implements the channel used by headquarters and space explorers to communicate.
 */
public class CommunicationChannel {

	/**
	 * Creates a {@code CommunicationChannel} object.
	 */
	
	/* I will implement the Multiple Producers Multiple Consumers design,
	 * both HQS and SpaceExplorers having different roles at different times:
	 * when HQS send the list of the new wormholes to the Space Explorers it means
	 * that HQS are the producers and Space Explorers are consumers, otherwise, when
	 * Space Explorers finish decoding the new frequency and send the data to HQS it means
	 * that Space Explorers are now the producers and HQS are the consumers.
	 * I will use two blocking queues in order to create this bidirectional communication.
	 */
	
	private BlockingQueue<Message> hQProduceSpaceExConsume;
	private BlockingQueue<Message> spaceExProduceHQConsume;
	private static final int CAPACITY = 10000;

	// each HQ thread will keep locked the lock until they put two messages
	private ReentrantLock lockWrite;
	// each SpaceExplorer will keep locked the lock until they read two messages
	private ReentrantLock lockRead;

	public CommunicationChannel() {

		hQProduceSpaceExConsume = new ArrayBlockingQueue<>(CAPACITY);
		spaceExProduceHQConsume = new ArrayBlockingQueue<>(CAPACITY);
		lockWrite = new ReentrantLock();
		lockRead = new ReentrantLock();
	}

	/**
	 * Puts a message on the space explorer channel (i.e., where space explorers write to and 
	 * headquarters read from).
	 * 
	 * @param message
	 *            message to be put on the channel
	 */
	public void putMessageSpaceExplorerChannel(Message message) {
		try {
			spaceExProduceHQConsume.put(message);
		} catch (InterruptedException ex) {
		}
	}

	/**
	 * Gets a message from the space explorer channel (i.e., where space explorers write to and
	 * headquarters read from).
	 * 
	 * @return message from the space explorer channel
	 */
	public Message getMessageSpaceExplorerChannel() {
		
		Message message = null;
		
		try {
			message = spaceExProduceHQConsume.take();
		} catch (InterruptedException ex) {
		}
		
		return message;
	}

	/**
	 * Puts a message on the headquarters channel (i.e., where headquarters write to and 
	 * space explorers read from).
	 * 
	 * @param message
	 *            message to be put on the channel
	 */
	public void putMessageHeadQuarterChannel(Message message) {

		   /* HQs will send pairs like:
			  -discovered solar system
			  -adjacent undiscovered solar system 1
			  -discovered solar system
			  -adjacent undiscovered solar system 2
			  - etc
		  */

			// ignore END and EXIT messages
		   if (!message.getData().equals("END") && !message.getData().equals("EXIT")) {
			 // will lock the reentrant lock for two times, such that one HQ put exactly two messages
			 lockWrite.lock();
				try {
				     hQProduceSpaceExConsume.add(message);
				} catch (Exception ex) {

				} finally {
				    // if the current HQ locked for two times, it means it put two messages on channel
					if (lockWrite.getHoldCount() == 2) {
					    // unlock to let a new HQ to write messages
					    lockWrite.unlock();
					    lockWrite.unlock();

					}
				}

		    }
	}

	/**
	 * Gets a message from the headquarters channel (i.e., where headquarters write to and
	 * space explorer read from).
	 * 
	 * @return message from the header quarter channel
	 */
	public Message getMessageHeadQuarterChannel() {
		
		Message message = null;

		lockRead.lock();
		try {
		   message = hQProduceSpaceExConsume.take();

		} catch (InterruptedException ex) {

		} finally {
			if (lockRead.getHoldCount() == 2) {
				// let new space explorer to read
				lockRead.unlock();
				lockRead.unlock();

			}
		}

		return message;

	}
}
