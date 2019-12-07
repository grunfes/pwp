<script id="app-template" type="x-template">
  <div>
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
    <template v-else>
      <div>
        <component
          :is="component"
          :show="show"
          :matches="show.matches"
          :picks="picks"
          @game-start="handleGameStart"
          @game-end="handleGameEnd"
        />
      </div>
    </template>
  </div>
</script>

<script id="game-summary-template" type="x-template">
  <div>
    <h2>Game Summary</h2>
    <p>{{ picks }}</p>
  </div>
</script>

<script id="game-intro-template" type="x-template">
  <div>
    <h2>{{ show.title }}</h2>
    <p>{{ show.type.name }}</p>
    <p>{{ show.venue.name }}</p>

    <div class="button">
      <a href="#" @click.prevent="$emit('game-start')">Start</a>
    </div>
  </div>
</script>

<script id="countdown-template" type="x-template">
  <div>
    <interval v-if="!intervalEnded" :duration="duration" @complete="handleComplete">
      <template #default="{ elapsed }">
        Rumble in
        {{ elapsed }}
      </template>
    </interval>

    <slot v-else />
  </div>
</script>

<script id="game-template" type="x-template">
  <countdown :key="index" :duration="3">
    <div>
      <interval :duration="5" @complete="handleChange">
        <template #default="{ elapsed }">
          {{ elapsed }}
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
  </countdown>
</script>

<script id="match-template" type="x-template">
  <div>
    <h2>{{ title }}</h2>
        <team
          v-for="team in teams"
          v-model="selected"
          :key="team.id"
          :id="team.id"
          :checked="team.id === selected"
          :wrestlers="team.wrestlers"
          @input="handleTeamInput"
        >
          <template>
            v-if="shouldAppend(index)"
            #append
          >
            VS
          </template>
        </team>
  </div>
</script>

<script id="team-template" type="x-template">
  <div>
    <slot name="prepend">
      <input
        type="checkbox"
        :checked="checked"
        @input="handleInput"
      />
    </slot>

    <ul>
      <li
        v-for="wrestler in wrestlers"
      >
        {{ wrestler.title }}
      </li>
    </ul>

    <slot name="append" />
  </div>
</script>

<div id="daily_game"></div>
