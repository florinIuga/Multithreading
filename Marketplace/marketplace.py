"""
This module represents the Marketplace.

Computer Systems Architecture Course
Assignment 1
March 2020
"""
from threading import Lock

class Marketplace:
    """
    Class that represents the Marketplace. It's the central part of the implementation.
    The producers and consumers use its methods concurrently.
    """

    def __init__(self, queue_size_per_producer):
        """
        Constructor

        :type queue_size_per_producer: Int
        :param queue_size_per_producer: the maximum size of a queue associated with each producer
        """
        self.prod_id = -1
        self.cart_id = -1
        self.mutex = Lock()
        self.buf = []
        self.size_per_prod = queue_size_per_producer
        self.carts = []
        self.delete_from_id = -1

    def register_producer(self):
        """
        Returns an id for the producer that calls this.
        """
        with self.mutex:
            self.prod_id += 1

        self.buf.append([])
        return self.prod_id


    def publish(self, producer_id, product):
        """
        Adds the product provided by the producer to the marketplace

        :type producer_id: String
        :param producer_id: producer id

        :type product: Product
        :param product: the Product that will be published in the Marketplace

        :returns True or False. If the caller receives False, it should wait and then try again.
        """

        #First check if the producer_id's queue is full
        #If queue is full the product can't be published, else add it into the queue.

        with self.mutex:
            if len(self.buf[producer_id]) == self.size_per_prod:
                return False

        self.buf[producer_id].append(product)
        return True

    def new_cart(self):
        """
        Creates a new cart for the consumer

        :returns an int representing the cart_id
        """
        with self.mutex:
            self.cart_id += 1

        self.carts.append([])
        return self.cart_id

    #buf is a list of lists, containing the list for each producer. Look for
    #the searched product in each of these lists until the product is found. Add the product
    #into cart, make it unavailable for the rest of the customers and keep the id of the producer
    #whose queue contained the searched product in 'delete_from_id' variable

    def add_to_cart(self, cart_id, product):
        """
        Adds a product to the given cart. The method returns

        :type cart_id: Int
        :param cart_id: id cart

        :type product: Product
        :param product: the product to add to cart

        :returns True or False. If the caller receives False, it should wait and then try again
        """
        for i in range(len(self.buf)):
            for prod in self.buf[i]:
                if prod == product:
                    self.carts[cart_id].append(product)
                    self.buf[i].remove(product)
                    with self.mutex:
                        self.delete_from_id = i
                    return True

        return False

    # Simply remove from cart_id list the product and add it to the producer's queue
    # where it had been previously found

    def remove_from_cart(self, cart_id, product):
        """
        Removes a product from cart.

        :type cart_id: Int
        :param cart_id: id cart

        :type product: Product
        :param product: the product to remove from cart
        """
        self.carts[cart_id].remove(product)
        self.buf[self.delete_from_id].append(product)


    def place_order(self, cart_id):
        """
        Return a list with all the products in the cart.

        :type cart_id: Int
        :param cart_id: id cart
        """
        return self.carts[cart_id]

