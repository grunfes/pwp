<script id="app-template" type="x-template">
  <div class="dailyGameRoot">
    <template v-if="loading">
      Loading...
    </template>
    <template v-else-if="error">
      <div class="messages error" style="display: block !important;">
        {{ error.message }}
      </div>
    </template>
    <template v-else-if="!hasDailyGame">
      <div>No Daily Game</div>
    </template>
    <template v-else-if="hasUserPlayed">
      <game-summary
        :summary="gameData.summary"
        :user="gameData.user"
      />
    </template>
    <template v-else>
      <transition name="slide-left" mode="out-in">
        <component
          :is="component"
          :show="gameData.show"
          :matches="gameData.show.matches"
          :user="gameData.user"
          :summary="summary"
          @game-start="handleGameStart"
          @game-end="handleGameEnd"
        />
      </transition>
    </template>
  </div>
</script>

<script id="game-summary-template" type="x-template">
  <div :class="klass" class="dailGameSummary dailyGame__content">
    <div class="dailyGameSummary__content">
      <h2>Game Summary</h2>
      <div class="dailyGameSummary_score">{{ user.name }} you <span v-if="summary.perfectGame">played perfectly</span> and scored {{ summary.score }} points in total</div>
      <ul>
        <li v-for="(item, index) in summary.picks" :key="index">
          {{ item }}
        </li>
      </ul>
    </div>
  </div>
</script>

<script id="game-intro-template" type="x-template">
  <div class="dailyGameIntro dailyGame__content">
    <div class="dailyGameIntro__content">
      <h2>{{ show.title }}</h2>
      <p v-if="show.type">{{ show.type.name }}</p>
      <p v-if="show.venue">{{ show.venue.name }}</p>

      <div class="button">
        <a href="#" @click.prevent="$emit('game-start')">Start</a>
      </div>
    </div>
  </div>
</script>

<script id="countdown-template" type="x-template">
  <div class="dailyGameCountdown">
    <interval :duration="duration" @complete="handleComplete">
      <template #default="{ elapsed }">
        <div class="dailyGameCountdown__content dailyGame__content">
          <countdown-animation :countdown="get(elapsed)" />
        </div>
      </template>t
    </interval>
  </div>
</script>

<script id="countdown-animation-template" type="x-template">
  <div class="dailyGameCountdownAnimated">
    <div ref="countdownEl" class="dailyGameCountdownAnimated__element">{{ countdown }}</div>
  </div>
</script>

<script id="game-template" type="x-template">
  <div>
    <transition name="slide-top" mode="out-in">
      <countdown v-if="isCountdown" :key="index" :duration="3" @complete="handleCountdownComplete" />
      <div v-else class="dailyGameShow">
        <h2 class="dailyGameMatchTitle">
          {{ match.title }}
        </h2>
        <interval :duration="5" @complete="handleChange">
          <template #default="{ elapsed }">
            <progress-bar :duration="5 * 1000" :progress="elapsed" />
          </template>
        </interval>

        <match
          :key="match.id"
          :id="match.id"
          :title="match.title"
          :teams="match.teams"
          @pick="handleTeamPick"
        />
      </div>
    </transition>
  </div>
</script>

<script id="match-template" type="x-template">
  <div class="dailyGameMatch dailyGame__content">
    <div class="dailyGameMatch__content">
      <team
        v-for="(team, index) in teams"
        v-model="selected"
        :key="team.id"
        :id="team.id"
        :checked="team.id === selected"
        :wrestlers="team.wrestlers"
        @input="handleTeamInput"
      >
        <template
          v-if="shouldAppend(index)"
          #append
        >
          <div class="dailyGameTeam_append">
            VS
          </div>
        </template>
      </team>
    </div>
  </div>
</script>

<script id="team-template" type="x-template">
  <div class="dailyGameTeam" :class="checkedCls">
    <div class="dailyGameTeam__selector">
      <slot name="prepend">
        <input
          type="checkbox"
          :checked="checked"
          @click="handleInput"
        />
      </slot>

      <ul class="dailyGameTeamWrestlers">
        <li
          v-for="wrestler in wrestlers"
          @click="handleInput"
        >
          {{ wrestler.title }}
        </li>
      </ul>
    </div>

    <slot name="append" />
  </div>
</script>

<script id="progress-bar-template" type="x-template">
  <div class="dailyGameProgress">
    <div class="dailyGameProgress__inner" :style="style" />
  </div>
</script>

<div id="daily_game"></div>
