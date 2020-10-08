"""
This module represents the Producer.

Computer Systems Architecture Course
Assignment 1
March 2020
"""

from threading import Thread
from time import sleep


class Producer(Thread):
    """
    Class that represents a producer.
    """

    def __init__(self, products, marketplace, republish_wait_time, **kwargs):
        """
        Constructor.

        @type products: List()
        @param products: a list of products that the producer will produce

        @type marketplace: Marketplace
        @param marketplace: a reference to the marketplace

        @type republish_wait_time: Time
        @param republish_wait_time: the number of seconds that a producer must
        wait until the marketplace becomes available

        @type kwargs:
        @param kwargs: other arguments that are passed to the Thread's __init__()
        """
        Thread.__init__(self, **kwargs)
        self.products = products
        self.marketplace = marketplace
        self.republish_wait_time = republish_wait_time
        self.producer_id = -1

    # For each product from products, publish it into the queue or wait
    # until the queue has a new free spot. If a producer publishes 5 products of a
    # certain type, the call publish() for 5 times.

    def run(self):
        self.producer_id = self.marketplace.register_producer()

        while True:
            for prod in self.products:
                for _ in range(0, prod[1]):
                    while not self.marketplace.publish(self.producer_id, prod[0]):
                        sleep(self.republish_wait_time)
                    sleep(prod[2])


