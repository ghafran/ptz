'use strict';

var $ = require('gulp-load-plugins')(),
    gulp = require('gulp'),
    chalk = require('chalk'),
    runSequence = require('run-sequence');

gulp.task('default', function () {
    runSequence(
        'lint', 'unit', 'watch'
    );
});

gulp.task('test', function () {
    runSequence(
        'lint', 'unit'
    );
});

gulp.task('lint', function () {
    return gulp.src(['**/*.js', '!node_modules/**/*'])
        .pipe($.jshint('.jshintrc'))
        .pipe($.jshint.reporter('jshint-stylish'))
        .pipe($.jshint.reporter('fail'));
});

gulp.task('unit', function () {
    return gulp
        .src(['test/**/*.js', '!node_modules/**/*'])
        .pipe($.mocha({
            reporter: 'spec'
        }));
});

gulp.task('build', $.shell.task(['node-gyp build']));

gulp.task('watch', function () {
    gulp.watch(['lib/*.cc'], {
        interval: 200
    }, ['build']).on('change', function (file) {
        console.log(chalk.underline.bold.blue(file.path), 'changed');
    });

    gulp.watch(['lib/*.js', 'test/**/*.js'], {
        interval: 200
    }, ['unit']).on('change', function (file) {
        console.log(chalk.underline.bold.blue(file.path), 'changed');
    });
});
