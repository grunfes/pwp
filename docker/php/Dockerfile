FROM php:5-fpm-alpine

ENV COMPOSER_ALLOW_SUPERUSER 1

RUN apk update \
	&& apk add --no-cache --virtual build-dependencies icu-dev g++ make autoconf \
	&& docker-php-source extract \
	&& pecl install xdebug-2.5.5 \
	&& docker-php-ext-enable xdebug \
	&& docker-php-source delete \
	&& apk add \
		freetype-dev \
		libjpeg-turbo-dev \
		libpng-dev \
	&& docker-php-ext-configure gd --with-freetype-dir=/usr/include/ --with-jpeg-dir=/usr/include/ \
	&& docker-php-ext-install -j$(nproc) gd \
	&& docker-php-ext-install pdo pdo_mysql \
	&& apk del build-dependencies \
	&& curl -sS https://getcomposer.org/installer | php -- --install-dir=/usr/local/bin --filename=composer \
	&& rm -rf /tmp/*

COPY xdebug.ini /usr/local/etc/php/conf.d/xdebug.ini

WORKDIR /var/www/html