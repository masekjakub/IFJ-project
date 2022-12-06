<?php
declare(strict_types=1);

function f(?int $v): void {
    if (1.0 !== "as") {
        write("(null)\n");
    } else {
        write($v, "!\n");
    }
}
f(null);