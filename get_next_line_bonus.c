/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-hyna <mel-hyna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 22:08:07 by mel-hyna          #+#    #+#             */
/*   Updated: 2025/11/29 22:08:07 by mel-hyna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static char	*free_all(char *a, char *b)
{
	if (a)
		free(a);
	if (b)
		free(b);
	return (NULL);
}

static char	*read_join(int fd, char *rest)
{
	char	*buf;
	int		r;

	buf = malloc(BUFFER_SIZE + 1);
	if (!buf)
		return (NULL);
	r = 1;
	while (r > 0 && (!rest || !ft_strchr(rest, '\n')))
	{
		r = read(fd, buf, BUFFER_SIZE);
		if (r < 0)
			return (free_all(buf, rest));
		buf[r] = '\0';
		rest = ft_strjoin(rest, buf);
		if (!rest)
			return (free_all(buf, NULL));
	}
	free(buf);
	return (rest);
}

static char	*new_rest(char *rest, int i)
{
	int		len;
	char	*next;
	int		j;

	len = ft_strlen(rest);
	if (i >= len)
		return (NULL);
	next = malloc(len - i + 1);
	if (!next)
		return (NULL);
	j = 0;
	while (rest[i + j])
	{
		next[j] = rest[i + j];
		j++;
	}
	next[j] = '\0';
	return (next);
}

static char	*extract_line(char **rest)
{
	int		i;
	int		j;
	char	*line;
	char	*next;

	if (!*rest || !**rest)
		return (gnl_free_rest(rest));
	i = 0;
	while ((*rest)[i] && (*rest)[i] != '\n')
		i++;
	if ((*rest)[i] == '\n')
		i++;
	line = malloc(i + 1);
	if (!line)
		return (gnl_free_rest(rest));
	j = -1;
	while (++j < i)
		line[j] = (*rest)[j];
	line[i] = '\0';
	next = new_rest(*rest, i);
	free(*rest);
	*rest = next;
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*rest[10240];
	char		*line;

	if (fd < 0 || fd > 10240 || BUFFER_SIZE <= 0)
		return (NULL);
	rest[fd] = read_join(fd, rest[fd]);
	if (!rest[fd])
		return (NULL);
	line = extract_line(&rest[fd]);
	return (line);
}
