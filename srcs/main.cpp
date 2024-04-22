/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:50:37 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:25:31 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Command.hpp"
#include "../includes/Client.hpp"
#include "../includes/Define.hpp"
#include "../includes/Channel.hpp"

int	verify_Input(int ac, char **av)
{

	if (ac != 3)
	{
		std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
		return (0);
	}
	else
	{
		char		*endptr;
		long		port = strtol(av[1], &endptr, 10);
		std::string	pass(av[2]);
		if (*endptr != '\0' || port < 1024 || port > 49151)
		{
			std::cout << "Invalid port" << std::endl;
			return (0);
		}

		if (pass.length() < 1)
		{
			std::cout << "Invalid password" << std::endl;
			return (0);
		}
	}
	return (1);
}

int	main(int ac, char **av)
{
	if (!verify_Input(ac, av))
		return (1);

	Server	server(atoi(av[1]), av[2]);

	try
	{
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}