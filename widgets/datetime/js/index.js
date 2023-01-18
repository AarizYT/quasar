function update() {
  document.getElementById("clock").innerHTML = dayjs().format(
    '[<div class="time">]h:mm:ss[<span class="ap">]A[</span>][</div><div class="day">]D[</div><div class="monthday">]MMMM YYYY<br>dddd[</div>]',
  );
}

setInterval(update, 1000);
