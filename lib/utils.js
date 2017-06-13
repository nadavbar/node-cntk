function argmax_1d(arr) {
    if (arr.length == 0) {
        return -1;
    }

    maxValue = arr[0];
    maxValueId = 0;

    for (var i=1; i <arr.length; i++) {
        if (arr[i] > maxValue) {
            maxValue = arr[i];
            maxValueId = i;
        }
    }

    return maxValueId;
}


function argmax(arr) {
    if (arr.length == 0) {
        return -1;
    }

    // if arr is an array of arrays:
    if (arr[0].length) {
        idx = [];
        for (var i =0; i < arr.length; i++) {
            idx.push(argmax_1d(arr[i]));
        }
        return idx;
    }

    return argmax_1d(arr);
}


exports.argmax = argmax;